#include "magnetometer.h"

volatile uint8_t interval_do_magnetometer_reading = 0x00;		//Mailbox value, set in timer2 ISR and read in delay

#define MAGNETOMETER_ADDRESS 0x1E
#define ALPHA 0.2

#if MAGNETOMETER == 1
static int16_t filtered[2];
static int16_t offsets[2];

void magnetometer_init(){
	int16_t x = eeprom_read_word((uint16_t*) MAGNETOMETER_EEPROM_BASE);
	int16_t y = eeprom_read_word((uint16_t*) (MAGNETOMETER_EEPROM_BASE + MAGNETOMETER_EEPROM_OFFSET));
	magnetometer_set_offsets(x, y);

	twi_init();
	
	uint8_t message[6];
	message[0] = 0x00;		//Register 0
	message[1] = 0x78;		//8 sample average, 75 Hz, normal mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x01;		//Register 1
	message[1] = 0x20;		//Gain 1 (± 1.3 Ga)
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x02;		//Register 2
	message[1] = 0x00;		//Continuous measurement mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);

	//Discard the first reading after mode change
	message[0] = 0x03;
	twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
	delay_ms(100);
	message[0] = 0x06;
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);

	//Read an actual value
	message[0] = 0x03;
	twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
	delay_ms(100);
	message[0] = 0x06;
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);

	//Initialize the filtered values
	filtered[0] = (((uint16_t) message[0]) << 8 | message[1]) + offsets[0];
	filtered[1] = (((uint16_t) message[4]) << 8 | message[5]) + offsets[1];
}

void magnetometer_get_offsets(int16_t *x, int16_t *y){
	*x = offsets[0];
	*y = offsets[1];
}

void magnetometer_set_offsets(int16_t x, int16_t y){
	offsets[0] = x;
	offsets[1] = y;
}

void magnetometer_get_raw(int16_t *x, int16_t *y){
	//Read 6 bytes starting at X MSB (0x03, reset to here prior to this invocation)
	uint8_t message[6];
	message[0] = 0x06;
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);
	
	*x = (((uint16_t) message[0]) << 8 | message[1]);
	*y = (((uint16_t) message[4]) << 8 | message[5]);
	
	//Reset pointer to read next time
	message[0] = 0x03;
	twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
}

void magnetometer_take_reading(){
	int16_t x = 0;
	int16_t y = 0;
	
	magnetometer_get_raw(&x, &y);
	
	x += offsets[0];
	y += offsets[1];
	
	//From what I have seen, spurious readings (saturation?) seem to show up as very large values
	// in the thousands... most normal readings seem to be in the -250..250 range.  By filtering
	// out any readings outside of |500| we should eliminate most (all?) of the spurious readings.
	if (x >= -500 && x <= 500 && y >= -500 && y <= 500){
		filtered[0] = ALPHA * x + (1 - ALPHA) * filtered[0];
		filtered[1] = ALPHA * y + (1 - ALPHA) * filtered[1];
	}
}

double magnetometer_read_heading(){
	return atan2(filtered[1], filtered[0]);
}
#else

void magnetometer_init(){}

void magnetometer_get_offsets(int16_t *x, int16_t *y){}

void magnetometer_set_offsets(int16_t x, int16_t y){}

void magnetometer_get_raw(int16_t *x, int16_t *y){}

double magnetometer_read_heading(){ return 0; }

#endif