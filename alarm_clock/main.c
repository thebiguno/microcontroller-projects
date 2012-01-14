#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#ifdef DEBUG
#include "lib/serial/serial.h"
#endif
#include "lib/i2c/i2c_master.h"

#define SHIFT_PORT					PORTD
#define SHIFT_DATA_PIN				PIND5
#define SHIFT_CLOCK_PIN				PIND6
#define SHIFT_LATCH_PIN				PIND7

#define I2C_ADDRESS					0x51

#define BUFFER_REFRESH				512
#define DIMMER_REFRESH				0

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

void shift_init(){
	//Enable outputs for shift register (clock LED driver)
	DDRD |= _BV(SHIFT_DATA_PIN) | _BV(SHIFT_CLOCK_PIN) | _BV(SHIFT_LATCH_PIN);
}

void shift_out(uint8_t data){
	for (int i = 0; i < 8; i++){
		//Clear the data pin first...
		SHIFT_PORT &= ~_BV(SHIFT_DATA_PIN);
		//... then set the bit (if appropriate).  We could probably
		// do this in one step, but this is more clear, plus speed is 
		// (probably) not critical here.
		SHIFT_PORT |= (((data >> (7 - i)) & 0x1) << SHIFT_DATA_PIN);
		
		//Pulse clock to shift in
		SHIFT_PORT &= ~_BV(SHIFT_CLOCK_PIN);
		SHIFT_PORT |= _BV(SHIFT_CLOCK_PIN);
	}
}

void shift_latch_data(){
	//Pulse latch to transfer contents to output
	SHIFT_PORT &= ~_BV(SHIFT_LATCH_PIN);	
	SHIFT_PORT |= _BV(SHIFT_LATCH_PIN);
}

void get_shift_data(uint8_t hours, uint8_t minutes, uint16_t *data1, uint16_t *data2){	
	//We have four digits: H2, H1, M2, M1.  (H2 is MSB of Hour).  Each of these digits has
	// 7 segments: A .. F.  Each segment is lit by a (pin,pin) combination, with the pin
	// numbers mapping to the LED panel pins, with the first
	// pin being the cathode (pin 1 or 2), and the second pin being the anode pin.  We 
	// map these combinations out below, above the defines for each digit.

	//Here we map between LED pins to shift register outputs, counting down from bit 14: 
	// LED		Shift Data		Shift Pin
	// 6		13				5
	// 7		12				4
	// 8		11				3
	// 9		10				2
	// 10		9				1
	// 12		8				15
	// 13		7				7
	// 15		6				6
	// 16		5				5
	// 17		4				4
	// 18		3				3
	// 19		2				2
	// 20		1				1
	// 21		0				15
	
	#define CATHODE_1	_BV(15)
	#define CATHODE_2	_BV(14)
	
	//    HourMinNumber_Segment_Cathode
	// H2: A=(1,7), B=(2,6), C=(2,9), D=(1,8), E=(2,8), G=(2,7) (F is not implemented, so 0, 4, 5, 6, 8, 9 are not possible)
	#define H2_A_1 	_BV(12)
	#define H2_A_2 	0
	#define H2_B_1 	0
	#define H2_B_2	_BV(13)
	#define H2_C_1	0
	#define H2_C_2	_BV(10)
	#define H2_D_1	_BV(11)
	#define H2_D_2	0
	#define H2_E_1	0
	#define H2_E_2	_BV(11)
	#define H2_F_1	0
	#define H2_F_2	0
	#define H2_G_1	0
	#define H2_G_2	_BV(12)
	
	// H1: A=(2,13), B=(2,10), C=(2,12), D=(1,12), E=(1,9), F=(1,13), G=(1,10)
	#define H1_A_1	0
	#define H1_A_2	_BV(7)
	#define H1_B_1	0
	#define H1_B_2	_BV(9)
	#define H1_C_1	0
	#define H1_C_2	_BV(8)
	#define H1_D_1	_BV(8)
	#define H1_D_2	0
	#define H1_E_1	_BV(10)
	#define H1_E_2	0
	#define H1_F_1	_BV(7)
	#define H1_F_2	0
	#define H1_G_1	_BV(9)
	#define H1_G_2	0
	
	// M2: A=(1,15), B=(1,16), C=(1,17), F=(2,15), G=(2,16), D=(2,17), E=(2,18)
	#define M2_A_1	_BV(6)
	#define M2_A_2	0
	#define M2_B_1	_BV(5)
	#define M2_B_2	0
	#define M2_C_1	_BV(4)
	#define M2_C_2	0
	#define M2_D_1	0
	#define M2_D_2	_BV(4)
	#define M2_E_1	0
	#define M2_E_2	_BV(3)
	#define M2_F_1	0
	#define M2_F_2	_BV(6)
	#define M2_G_1	0
	#define M2_G_2	_BV(5)

	// M1: E=(1,18), G=(1,19), D=(1,20), F=(1,21), B=(2,19), C=(2,20), A=(2,21)
	#define M1_A_1	0
	#define M1_A_2	_BV(0)
	#define M1_B_1	0
	#define M1_B_2	_BV(2)
	#define M1_C_1	0
	#define M1_C_2	_BV(1)
	#define M1_D_1	_BV(1)
	#define M1_D_2	0
	#define M1_E_1	_BV(3)
	#define M1_E_2	0
	#define M1_F_1	_BV(0)
	#define M1_F_2	0
	#define M1_G_1	_BV(2)
	#define M1_G_2	0

	
	//Clears existing data, and set the cathode to use
	*data1 = CATHODE_1;
	*data2 = CATHODE_2;
	
	//Set MSB Hour; only 1, 2, and 3 are supported.
	switch (hours >> 4) {
		case 0x01:
			*data1 |= H2_B_1 | H2_C_1;
			*data2 |= H2_B_2 | H2_C_2;
			break;
			
		case 0x02:
			*data1 |= H2_A_1 | H2_B_1 | H2_D_1 | H2_E_1 | H2_G_1;
			*data2 |= H2_A_2 | H2_B_2 | H2_D_2 | H2_E_2 | H2_G_2;
			break;

		case 0x03:
			*data1 |= H2_A_1 | H2_B_1 | H2_C_1 | H2_D_1 | H2_G_1;
			*data2 |= H2_A_2 | H2_B_2 | H2_C_2 | H2_D_2 | H2_G_2;
			break;
	}
	
	//Set LSB hour
	switch (hours & 0xF) {
		case 0x00:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2;
			break;

		case 0x01:
			*data1 |= H1_B_1 | H1_C_1;
			*data2 |= H1_B_2 | H1_C_2;
			break;
			
		case 0x02:
			*data1 |= H1_A_1 | H1_B_1 | H1_D_1 | H1_E_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_D_2 | H1_E_2 | H1_G_2;
			break;

		case 0x03:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_G_2;
			break;

		case 0x04:
			*data1 |= H1_B_1 | H1_C_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_B_2 | H1_C_2 | H1_F_2 | H1_G_2;
			break;

		case 0x05:
			*data1 |= H1_A_1 | H1_C_1 | H1_D_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_C_2 | H1_D_2 | H1_F_2 | H1_G_2;
			break;

		case 0x06:
			*data1 |= H1_A_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x07:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2;
			break;

		case 0x08:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x09:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_D_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_D_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0A:
			*data1 |= H1_A_1 | H1_B_1 | H1_C_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_B_2 | H1_C_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0B:
			*data1 |= H1_C_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_C_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0C:
			*data1 |= H1_A_1 | H1_D_1 | H1_E_1 | H1_F_1;
			*data2 |= H1_A_2 | H1_D_2 | H1_E_2 | H1_F_2;
			break;

		case 0x0D:
			*data1 |= H1_B_1 | H1_C_1 | H1_D_1 | H1_E_1 | H1_G_1;
			*data2 |= H1_B_2 | H1_C_2 | H1_D_2 | H1_E_2 | H1_G_2;
			break;

		case 0x0E:
			*data1 |= H1_A_1 | H1_D_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_D_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

		case 0x0F:
			*data1 |= H1_A_1 | H1_E_1 | H1_F_1 | H1_G_1;
			*data2 |= H1_A_2 | H1_E_2 | H1_F_2 | H1_G_2;
			break;

	}
			
	//Set MSB minute
	switch (minutes >> 4) {
		case 0x00:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2;
			break;

		case 0x01:
			*data1 |= M2_B_1 | M2_C_1;
			*data2 |= M2_B_2 | M2_C_2;
			break;
			
		case 0x02:
			*data1 |= M2_A_1 | M2_B_1 | M2_D_1 | M2_E_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_D_2 | M2_E_2 | M2_G_2;
			break;

		case 0x03:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_G_2;
			break;

		case 0x04:
			*data1 |= M2_B_1 | M2_C_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_B_2 | M2_C_2 | M2_F_2 | M2_G_2;
			break;

		case 0x05:
			*data1 |= M2_A_1 | M2_C_1 | M2_D_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_C_2 | M2_D_2 | M2_F_2 | M2_G_2;
			break;

		case 0x06:
			*data1 |= M2_A_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x07:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2;
			break;

		case 0x08:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x09:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_D_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_D_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0A:
			*data1 |= M2_A_1 | M2_B_1 | M2_C_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_B_2 | M2_C_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0B:
			*data1 |= M2_C_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_C_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0C:
			*data1 |= M2_A_1 | M2_D_1 | M2_E_1 | M2_F_1;
			*data2 |= M2_A_2 | M2_D_2 | M2_E_2 | M2_F_2;
			break;

		case 0x0D:
			*data1 |= M2_B_1 | M2_C_1 | M2_D_1 | M2_E_1 | M2_G_1;
			*data2 |= M2_B_2 | M2_C_2 | M2_D_2 | M2_E_2 | M2_G_2;
			break;

		case 0x0E:
			*data1 |= M2_A_1 | M2_D_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_D_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;

		case 0x0F:
			*data1 |= M2_A_1 | M2_E_1 | M2_F_1 | M2_G_1;
			*data2 |= M2_A_2 | M2_E_2 | M2_F_2 | M2_G_2;
			break;
	}
			
	//Set LSB minute
	switch (minutes & 0x0F) {
		case 0x00:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2;
			break;

		case 0x01:
			*data1 |= M1_B_1 | M1_C_1;
			*data2 |= M1_B_2 | M1_C_2;
			break;
			
		case 0x02:
			*data1 |= M1_A_1 | M1_B_1 | M1_D_1 | M1_E_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_D_2 | M1_E_2 | M1_G_2;
			break;

		case 0x03:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_G_2;
			break;

		case 0x04:
			*data1 |= M1_B_1 | M1_C_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_B_2 | M1_C_2 | M1_F_2 | M1_G_2;
			break;

		case 0x05:
			*data1 |= M1_A_1 | M1_C_1 | M1_D_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_C_2 | M1_D_2 | M1_F_2 | M1_G_2;
			break;

		case 0x06:
			*data1 |= M1_A_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x07:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2;
			break;

		case 0x08:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x09:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_D_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_D_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0A:
			*data1 |= M1_A_1 | M1_B_1 | M1_C_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_B_2 | M1_C_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0B:
			*data1 |= M1_C_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_C_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0C:
			*data1 |= M1_A_1 | M1_D_1 | M1_E_1 | M1_F_1;
			*data2 |= M1_A_2 | M1_D_2 | M1_E_2 | M1_F_2;
			break;

		case 0x0D:
			*data1 |= M1_B_1 | M1_C_1 | M1_D_1 | M1_E_1 | M1_G_1;
			*data2 |= M1_B_2 | M1_C_2 | M1_D_2 | M1_E_2 | M1_G_2;
			break;

		case 0x0E:
			*data1 |= M1_A_1 | M1_D_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_D_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;

		case 0x0F:
			*data1 |= M1_A_1 | M1_E_1 | M1_F_1 | M1_G_1;
			*data2 |= M1_A_2 | M1_E_2 | M1_F_2 | M1_G_2;
			break;
		
	}
}

/*
 * Validates that the input is a valid time format, and fixes it if possible.  Also converts between 12 and 24 hour format.
 */
void format_time(uint8_t *hours, uint8_t *minutes, uint8_t time_format){
	if (time_format == TIME_FORMAT_24){
		while (*hours >= 0x24) *hours -= 0x24;	//Don't allow for hours outside of [0..23]
	}
	else if (time_format == TIME_FORMAT_12){
		if (*hours == 0x00) *hours = 0x12;	//0 == midnight == 12:00
		while (*hours > 0x12) *hours -= 0x12; //Convert to 12 hour format
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
}

/*
 * Reads the current time from RTC
 */
void get_current_time(uint8_t *hours, uint8_t *minutes){
	//Reset the register to reading time
	message[0] = I2C_ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x02; //Reset register pointer
	i2c_start_transceiver_with_data(message, 2);

	message[0] = I2C_ADDRESS << 1 | I2C_READ;
	i2c_start_transceiver_with_data(message, 4);
	i2c_get_data_from_transceiver(message, 4);
	
	*hours = message[3];
	*minutes = message[2];
}

/*
 * Returns the current time for the given alarm index.
 */
void get_current_alarm(uint8_t alarm_index, uint8_t *hours, uint8_t *minutes){
	*hours = alarms[alarm_index][ALARM_HOURS_INDEX];
	*minutes = alarms[alarm_index][ALARM_MINUTES_INDEX];
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
	
	shift_latch_data();
}

int main (void){
#ifdef DEBUG
	//Init hardware
	serial_init_b(9600);
#endif
	i2c_master_init(400);
	i2c_clock_init();
	shift_init();
	
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
	
	uint16_t buffer_refresh_counter = 0;
	uint16_t dimmer_refresh_counter = 0;
	
	while (1){
		//Check button state to change mode if needed
		//TODO
	
		//Calculate the data for the display buffer.  We only want to do this every X iterations.
		if (buffer_refresh_counter >= BUFFER_REFRESH){
			buffer_refresh_counter = 0;
			if (mode == MODE_DEFAULT || mode == MODE_SET_TIME){
				//Get the current time from the RTC
				get_current_time(&hours, &minutes);
				
				//Convert to proper time format
				format_time(&hours, &minutes, TIME_FORMAT);
				
				//Populate the two shift data (display buffer) variables
				get_shift_data(hours, minutes, &shift_data1, &shift_data2);
			}
			else if (mode == MODE_SET_ALARM){
				//Get the current time from the RTC
				get_current_alarm(0, &hours, &minutes);
				
				//Convert to proper time format
				format_time(&hours, &minutes, TIME_FORMAT);
				
				//Populate the two shift data (display buffer) variables
				get_shift_data(hours, minutes, &shift_data1, &shift_data2);
				
			}
		}
		
		//We always want to refresh the display
		if (dimmer_refresh_counter > DIMMER_REFRESH){
			dimmer_refresh_counter = 0;
			refresh_display(shift_data1, shift_data2);
		}

		buffer_refresh_counter++;
		dimmer_refresh_counter++;
	}
}

