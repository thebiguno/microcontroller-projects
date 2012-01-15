#include <avr/io.h>
#include <stdlib.h>

#ifdef DEBUG
#include "lib/serial/serial.h"
#endif
#include "lib/i2c/i2c_master.h"


#include "button.h"
#include "shift.h"


#define I2C_ADDRESS					0x51

//Defines the LED brightness via PWM.  0 is max brightness, and higher numbers are dimmer.
#define DIMMER_MAX					0
//Defines (in number of main loop iterations) how often to refresh the display buffer.
#define BUFFER_REFRESH				1024
//Defines (in number of main loop iterations) how long to debounce buttons
#define BUTTON_DEBOUNCE				16
//Defines (in number of main loop iterations) how fast to repeat held-down buttons
#define BUTTON_REPEAT				2048

#define MODE_DEFAULT				0x00
#define MODE_SET_TIME				0x01
#define MODE_SET_ALARM				0x02

#define TIME_FORMAT_12				0x00
#define TIME_FORMAT_24				0x01
#define TIME_FORMAT					TIME_FORMAT_12

#define ALARM_MAX_COUNT				4

#define ALARM_HOURS_INDEX			0
#define ALARM_MINUTES_INDEX			1
#define ALARM_MODE_INDEX			2

#define ALARM_MODE_COUNT			3
#define ALARM_MODE_OFF				0x00
#define ALARM_MODE_BUZZER			0x01
#define ALARM_MODE_RADIO			0x02

//Used for i2c messages
uint8_t message[10];

//Used to store alarms.  First index is for alarm index (max of ALARM_MAX_COUNT).
// Second index is for alarm data (ALARM_HOURS_INDEX, ALARM_MINUTES_INDEX, 
// ALARM_MODE_INDEX)
uint8_t alarms[ALARM_MAX_COUNT][3];

#ifdef DEBUG
//Temp variable used for number to string conversions
char temp[30];
#endif

/*
 * Validates that the input is a valid time format, and fixes it if possible.  Also converts between 12 and 24 hour format.
 */
void format_time(uint8_t *hours, uint8_t *minutes, uint8_t *flags, uint8_t time_format){
	//Enable the colon, since this we are displaying time
	*flags = SHIFT_FLAG_COLON;
	
	if (time_format == TIME_FORMAT_24){
		if (*hours >= 0x24) *hours = 0x00;	//Don't allow for hours outside of [0..23]
	}
	else if (time_format == TIME_FORMAT_12){
		if (*hours >= 0x12) *flags |= SHIFT_FLAG_PM; //Set PM flag
		if (*hours == 0x00) *hours = 0x12;	//0 == midnight == 12:00
		if (*hours > 0x12){
			//Convert to 12 hour format
			if (*hours == 0x20 || *hours == 0x21) *hours = *hours - 0x18;
			else *hours = *hours - 0x12;
		}
	}
	
	if (*minutes > 0x59) *minutes = 0;
	
	//Check if there are any active alarms
	for (uint8_t i = 0; i < ALARM_MAX_COUNT; i++){
		if (alarms[i][ALARM_MODE_INDEX] != ALARM_MODE_OFF){
			*flags |= SHIFT_FLAG_AUTO;
		}
	}
}

void execute_alarm(uint8_t alarm_index){
	serial_write_s("Executing alarm #");
	serial_write_s(itoa(alarm_index, temp, 16));
	serial_write_s(", mode = ");
	if (alarms[alarm_index][ALARM_MODE_INDEX] == ALARM_MODE_BUZZER) serial_write_s("buzzer");
	else serial_write_s("radio");
	serial_write_s("\n\r");
}

/*
 * Sets up the RTC with desired values.
 */
void i2c_clock_init(){
	message[0] = I2C_ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x00; //Reset register pointer
	i2c_start_transceiver_with_data(message, 2);

	message[0] = I2C_ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x00;//Control Status 1
	message[2] = 0x00;//Control Status 2
	i2c_start_transceiver_with_data(message, 3);
	
#ifdef DEBUG

	//Set clock to known time for testing
	message[0] = I2C_ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x02; //Reset register pointer to 0x02
	message[2] = 0x56;	//56 seconds
	message[3] = 0x51;	//51 minutes
	message[4] = 0x18;	//18 hours
	message[5] = 0x07;	//7th
	message[6] = 0x04;	//Thursday
	message[7] = 0x01;	//January
	message[8] = 0x12;  //2012
	serial_write_s("Setting time...");
	i2c_start_transceiver_with_data(message, 9);
	serial_write_s("Done\n\r");
	
	//Set alarms to known time for testing
	alarms[0][ALARM_MINUTES_INDEX] = 0x52;
	alarms[0][ALARM_HOURS_INDEX] = 0x18;
	alarms[0][ALARM_MODE_INDEX] = ALARM_MODE_BUZZER;

	alarms[1][ALARM_MINUTES_INDEX] = 0x53;
	alarms[1][ALARM_HOURS_INDEX] = 0x18;
	alarms[1][ALARM_MODE_INDEX] = ALARM_MODE_RADIO;
#endif
}

/*
 * Reads the current time from RTC; if an alarm condition is reached, execute alarm.
 */
void get_time(uint8_t *hours, uint8_t *minutes){
	//Reset the register to reading time
	message[0] = I2C_ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x02; //Reset register pointer
	i2c_start_transceiver_with_data(message, 2);

	message[0] = I2C_ADDRESS << 1 | I2C_READ;
	i2c_start_transceiver_with_data(message, 4);
	i2c_get_data_from_transceiver(message, 4);
	
	uint8_t new_hours = message[3] & 0x3F;
	uint8_t new_minutes = message[2] & 0x7F;
	
	for (uint8_t i = 0; i < ALARM_MAX_COUNT; i++){
		//If the newly read time matches an alarm and the last time does 
		// not (i.e. it was just executed), then execute alarm
		if (alarms[i][ALARM_HOURS_INDEX] == new_hours 
				&& alarms[i][ALARM_HOURS_INDEX] != *hours
				&& alarms[i][ALARM_MINUTES_INDEX] == new_minutes 
				&& alarms[i][ALARM_MINUTES_INDEX] != *minutes
				&& alarms[i][ALARM_MODE_INDEX] != ALARM_MODE_OFF){
			execute_alarm(i);
		}
	}
	
	*hours = new_hours;
	*minutes = new_minutes;
}

/*
 * Increment the RTC chip by the given hours / minutes
 */
void set_time(uint8_t hours_offset, uint8_t minutes_offset){
	uint8_t hours = 0;
	uint8_t minutes = 0;
	
	get_time(&hours, &minutes);
	
	hours += hours_offset;
	minutes += minutes_offset;
	
	//Keep everything in valid BCD
	if ((minutes & 0xF) >= 0x0A) minutes += 0x06;
	if (minutes > 0x59) minutes = 0x00;
	
	if ((hours & 0xF) >= 0x0A) hours += 0x06;
	if (hours > 0x23) hours = 0;
	
	//Set clock to known time for testing
	message[0] = I2C_ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x02; //Reset register pointer to 0x02
	message[2] = 0x0;	//Reset seconds
	message[3] = minutes;
	message[4] = hours;
	i2c_start_transceiver_with_data(message, 5);
}

/*
 * Returns the current time for the given alarm index.
 */
void get_alarm(uint8_t alarm_index, uint8_t *hours, uint8_t *minutes){
	*hours = alarms[alarm_index][ALARM_HOURS_INDEX];
	*minutes = alarms[alarm_index][ALARM_MINUTES_INDEX];
}

/*
 * Increment the specified alarm by the given hours / minutes
 */
void set_alarm(uint8_t alarm_index, uint8_t hours_offset, uint8_t minutes_offset){
	uint8_t hours = 0;
	uint8_t minutes = 0;
	
	get_alarm(alarm_index, &hours, &minutes);
	
	hours += hours_offset;
	minutes += minutes_offset;
	
	//Keep everything in valid BCD
	if ((minutes & 0xF) >= 0x0A) minutes += 0x06;
	if (minutes > 0x59) minutes = 0x00;
	
	if ((hours & 0xF) >= 0x0A) hours += 0x06;
	if (hours > 0x23) hours = 0;
	
	alarms[alarm_index][ALARM_HOURS_INDEX] = hours;
	alarms[alarm_index][ALARM_MINUTES_INDEX] = minutes;
}

/*
 * Shifts out the display buffers to the shift registers; alternates between each set
 * of display data to keep all segments displayed.
 */
void refresh_display(uint32_t data1, uint32_t data2){
	static uint8_t cathode = 0;
	static uint8_t dimmer_counter = 0;
	
	if (dimmer_counter == 0){
		if (cathode == 0){
			shift_out((data1 >> 16) & 0xFF);
			shift_out((data1 >> 8) & 0xFF);
			shift_out(data1 & 0xFF);
			cathode = 1;
		}
		else {
			shift_out((data2 >> 16) & 0xFF);
			shift_out((data2 >> 8) & 0xFF);
			shift_out(data2 & 0xFF);
			cathode = 0;
		}
	}
	else {
		shift_out(0x00);
		shift_out(0x00);
		shift_out(0x00);
	}
		
	shift_latch();
	
	dimmer_counter++;
	if (dimmer_counter >= DIMMER_MAX) dimmer_counter = 0;
}


int main (void){
	DDRC = 0x00;
	PORTC = 0x00;

#ifdef DEBUG
	//Init hardware
	serial_init_b(9600);
#endif
	i2c_master_init(400);
	i2c_clock_init();
	shift_init();
	button_init();
	
	//The display buffers; each of these are shifted into the registers repeatedly to pulse back and
	// forth on the LED display.
	uint32_t shift_data1 = 0;
	uint32_t shift_data2 = 0;

	//Variables for LED digits 1/2 and 3/4, and other display flags (PM, colon, etc).  Mostly 
	// these are for display of hours / minutes, but that is not the only use (e.g. radio 
	// tuning, alarm index, etc).  These will be displayed on the LED as hex numbers, so for 
	// time it should be in BCD format.
	uint8_t digit12 = 0x00;
	uint8_t digit34 = 0x00;
	uint8_t flags = 0x00;
	
	//Button state.  Currently shares flag definitions with the button pints themselves, but 
	// this should change if the buttons move to various ports.
	uint8_t button_state = 0x00;
	uint8_t last_button_state = 0x00;
	
	uint16_t buffer_refresh_counter = 0x00;
	uint16_t button_debounce_counter = 0x00;
	uint16_t button_repeat_counter = 0x00;
	
	while (1){
		if (button_debounce_counter > 0) button_debounce_counter--;
		if (button_repeat_counter > 0) button_repeat_counter--;

		//Read buttons
		button_state = button_read();
	
		if (button_debounce_counter == 0x00 && button_repeat_counter == 0x00){
			//If a button is pressed, then check if it has just changed
			if (button_state != last_button_state){
				button_debounce_counter = BUTTON_DEBOUNCE;
				button_repeat_counter = 0x00;
				last_button_state = button_state;
	
				#ifdef DEBUG					
//				serial_write_s("Button State: ");
//				serial_write_s(itoa(button_state, temp, 16));
//				serial_write_s("\n\r");
				#endif
			}

			//Check repeat speed (repeat only applies to H / M / Sleep keys plus 
			// alarm or time modifiers)
			if (((button_state & BUTTON_ALARM) 
					|| (button_state & BUTTON_TIME))
					&& ((button_state & BUTTON_HOUR) 
						|| (button_state & BUTTON_MINUTE) 
						|| (button_state & BUTTON_SLEEP))){
				button_repeat_counter = BUTTON_REPEAT;
			}

			//If alarm is pressed, display / modify alarms
			if (button_state & BUTTON_ALARM){
				static uint8_t alarm_index = 0x00;
				
				//Alarm + Sleep = change alarm index
				if (button_state & BUTTON_SLEEP){
					if (button_state & BUTTON_HOUR){
						alarm_index--;
						if (alarm_index >= ALARM_MAX_COUNT) alarm_index = ALARM_MAX_COUNT - 1;
					}
					else if (button_state & BUTTON_MINUTE){
						alarm_index++;
						if (alarm_index >= ALARM_MAX_COUNT) alarm_index = 0;
					}

					digit12 = 0x0A;
					digit34 = alarm_index + 1;
					if (alarms[alarm_index][ALARM_MODE_INDEX] != ALARM_MODE_OFF) flags |= SHIFT_FLAG_AUTO;
					else flags = 0x00;
				}
				//Alarm + Snooze = view / change alarm mode
				else if (button_state & BUTTON_SNOOZE){
					if (button_state & BUTTON_HOUR){
						alarms[alarm_index][ALARM_MODE_INDEX]--;
						if (alarms[alarm_index][ALARM_MODE_INDEX] >= ALARM_MODE_RADIO) alarms[alarm_index][ALARM_MODE_INDEX] = ALARM_MODE_RADIO;
					}
					else if (button_state & BUTTON_MINUTE){
						alarms[alarm_index][ALARM_MODE_INDEX]++;
						if (alarms[alarm_index][ALARM_MODE_INDEX] > ALARM_MODE_RADIO) alarms[alarm_index][ALARM_MODE_INDEX] = ALARM_MODE_OFF;
					}

					digit12 = 0x0A;
					if (alarms[alarm_index][ALARM_MODE_INDEX] == ALARM_MODE_OFF){
						digit34 = 0x00;
						flags = 0x00;
					}
					else if (alarms[alarm_index][ALARM_MODE_INDEX] == ALARM_MODE_BUZZER){
						digit34 = 0x0B;
						flags = SHIFT_FLAG_AUTO;
					}
					else if (alarms[alarm_index][ALARM_MODE_INDEX] == ALARM_MODE_RADIO){
						digit34 = 0x0F;
						flags = SHIFT_FLAG_AUTO;
					}
					
				}
				//Alarm + Time = view / change alarm time
				else if (button_state & BUTTON_TIME){
					if (button_state & BUTTON_HOUR){
						set_alarm(alarm_index, 1, 0);					
					}
					else if (button_state & BUTTON_MINUTE){
						set_alarm(alarm_index, 0, 1);
					}
					//Refresh the display buffer with current alarm time
					get_alarm(alarm_index, &digit12, &digit34);
					format_time(&digit12, &digit34, &flags, TIME_FORMAT);					
				}
				//Alarm by itself = show current alarm index
				else {
					digit12 = 0x0A;
					digit34 = alarm_index + 1;
					if (alarms[alarm_index][ALARM_MODE_INDEX] != ALARM_MODE_OFF) flags |= SHIFT_FLAG_AUTO;
					else flags = 0x00;
				}
			}
			//If time is pressed modify time
			else if (button_state & BUTTON_TIME){
				if (button_state & BUTTON_HOUR){
					//Updates the time
					set_time(0x01, 0x00);					
				}
				else if (button_state & BUTTON_MINUTE){
					set_time(0x00, 0x01);
				}
			}
		}
		
		//If Alarm is not pressed, we can show time
		if (buffer_refresh_counter >= BUFFER_REFRESH && !(button_state & BUTTON_ALARM)){
			get_time(&digit12, &digit34);
			format_time(&digit12, &digit34, &flags, TIME_FORMAT);		
			buffer_refresh_counter = 0;
		}
		
		shift_format_data(digit12, digit34, flags, &shift_data1, &shift_data2);
		refresh_display(shift_data1, shift_data2);

		buffer_refresh_counter++;
	}
}

