#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#ifdef DEBUG
#include "lib/serial/serial.h"
#endif
#include "lib/i2c/i2c_master.h"


#include "button.h"
#include "shift.h"


#define I2C_ADDRESS					0x51

#define BUFFER_REFRESH				4092
#define DIMMER_REFRESH				0
#define BUTTON_REPEAT				2048

#define MODE_DEFAULT				0x00
#define MODE_SET_TIME				0x01
#define MODE_SET_ALARM				0x02

#define TIME_FORMAT_12				0x00
#define TIME_FORMAT_24				0x01
#define TIME_FORMAT					TIME_FORMAT_12

#define ALARM_MAX_COUNT				0x04

#define ALARM_HOURS_INDEX			0x00
#define ALARM_MINUTES_INDEX			0x01

//Used for i2c messages
uint8_t message[10];

//Used to store alarms.  First index is for alarm index (max of ALARM_MAX_COUNT).
// Second index is for alarm data (ALARM_*_INDEX)
uint8_t alarms[ALARM_MAX_COUNT][3];

#ifdef DEBUG
//Temp variable used for number to string conversions
char temp[30];
#endif

/*
 * Validates that the input is a valid time format, and fixes it if possible.  Also converts between 12 and 24 hour format.
 */
void format_time(uint8_t *hours, uint8_t *minutes, uint8_t time_format){
	if (time_format == TIME_FORMAT_24){
		if (*hours >= 0x24) *hours = 0x00;	//Don't allow for hours outside of [0..23]
	}
	else if (time_format == TIME_FORMAT_12){
		if (*hours == 0x00) *hours = 0x12;	//0 == midnight == 12:00
		if (*hours > 0x12){
			//Convert to 12 hour format
			if (*hours == 0x20 || *hours == 0x21) *hours = *hours - 0x18;
			else *hours = *hours - 0x12;
		}
	}
	
	if (*minutes > 0x59) *minutes = 0;
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

	alarms[1][ALARM_MINUTES_INDEX] = 0x53;
	alarms[1][ALARM_HOURS_INDEX] = 0x18;
#endif
}

void execute_alarm(uint8_t alarm_index){
	serial_write_s("Executing alarm #");
	serial_write_s(itoa(alarm_index, temp, 16));
	serial_write_s("\n\r");
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
				&& alarms[i][ALARM_MINUTES_INDEX] != *minutes){
			execute_alarm(i);
		}
	}
	
	*hours = new_hours;
	*minutes = new_minutes;
}

void set_time(int8_t hours_offset, int8_t minutes_offset){
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
void refresh_display(uint16_t data1, uint16_t data2){
	static uint8_t cathode = 0;
	
	if (cathode == 0){
		shift_out(data1 >> 8);
		shift_out(data1 & 0xFF);		
		cathode = 1;
	}
	else {
		shift_out(data2 >> 8);
		shift_out(data2 & 0xFF);
		cathode = 0;
	}
	
	shift_latch();
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
	
	//Clock mode: can switch between showing time, setting time, setting alarm, setting music, etc
	uint8_t mode = MODE_DEFAULT;

	//The display buffers; each of these are shifted into the registers repeatedly to pulse back and
	// forth on the LED display.
	uint16_t shift_data1 = 0;
	uint16_t shift_data2 = 0;

	//Hours / minutes variables.  Not strictly for use only to hold time (e.g. radio tuning
	// will use these as well).  These are stored as BCD numbers.
	uint8_t hours = 0;
	uint8_t minutes = 0;
	
	//Button state.  Currently shares flag definitions with the button pints themselves, but 
	// this should change if the buttons move to various ports.
	uint8_t button_state = 0;
	
	uint16_t buffer_refresh_counter = 0;
	uint16_t dimmer_refresh_counter = 0;
	uint16_t button_repeat_counter = 0;
	
	while (1){
		//Check button state to change mode if needed
		button_state = button_read();
		
		
		if (button_repeat_counter >= BUTTON_REPEAT){
			button_repeat_counter = 0;
			if (button_state & BUTTON_TIME && !(button_state & BUTTON_ALARM)){
				if (button_state & BUTTON_HOUR){
					//Updates the time
					set_time(1, 0);					
				}
				else if (button_state & BUTTON_MINUTE){
					set_time(0, 1);
				}
				
				//Refresh the display buffer
				get_time(&hours, &minutes);
				format_time(&hours, &minutes, TIME_FORMAT);
				shift_format_data(hours, minutes, &shift_data1, &shift_data2);
			}
			else if (button_state & BUTTON_ALARM){
				static uint8_t alarm_index = 0x00;
				if (button_state & BUTTON_HOUR){
					//Updates the time
					set_alarm(alarm_index, 1, 0);					

					//Refresh the display buffer
					get_alarm(alarm_index, &hours, &minutes);
					format_time(&hours, &minutes, TIME_FORMAT);
					shift_format_data(hours, minutes, &shift_data1, &shift_data2);
				}
				else if (button_state & BUTTON_MINUTE){
					set_alarm(alarm_index, 0, 1);

					//Refresh the display buffer
					get_alarm(alarm_index, &hours, &minutes);
					format_time(&hours, &minutes, TIME_FORMAT);
					shift_format_data(hours, minutes, &shift_data1, &shift_data2);
				}
				else if (button_state & BUTTON_SLEEP){
					alarm_index++;
					if (alarm_index >= ALARM_MAX_COUNT) alarm_index = 0;

					//Refresh the display buffer
					shift_format_data(0, alarm_index + 1, &shift_data1, &shift_data2);
				}
				else if (button_state & BUTTON_TIME){
					//Refresh the display buffer
					get_alarm(alarm_index, &hours, &minutes);
					format_time(&hours, &minutes, TIME_FORMAT);
					shift_format_data(hours, minutes, &shift_data1, &shift_data2);
				}
				else {
					//Refresh the display buffer to show the alarm index
					shift_format_data(0, alarm_index + 1, &shift_data1, &shift_data2);
				}
			}
		}
	
		//Calculate the data for the display buffer.  We only want to do this every X iterations.
		if (buffer_refresh_counter >= BUFFER_REFRESH && !(button_state & BUTTON_ALARM)){
			buffer_refresh_counter = 0;
			if (mode == MODE_DEFAULT || mode == MODE_SET_TIME){
				//Get the current time from the RTC
				get_time(&hours, &minutes);
				
				//Convert to proper time format
				format_time(&hours, &minutes, TIME_FORMAT);
				
				//Populate the two shift data (display buffer) variables
				shift_format_data(hours, minutes, &shift_data1, &shift_data2);
			}
			else if (mode == MODE_SET_ALARM){
				//Get the current time from the RTC
				get_alarm(0, &hours, &minutes);
				
				//Convert to proper time format
				format_time(&hours, &minutes, TIME_FORMAT);
				
				//Populate the two shift data (display buffer) variables
				shift_format_data(hours, minutes, &shift_data1, &shift_data2);
				
			}
		}
		
		//We always want to refresh the display
		if (dimmer_refresh_counter > DIMMER_REFRESH){
			dimmer_refresh_counter = 0;
			refresh_display(shift_data1, shift_data2);
		}

		buffer_refresh_counter++;
		dimmer_refresh_counter++;
		button_repeat_counter++;
	}
}

