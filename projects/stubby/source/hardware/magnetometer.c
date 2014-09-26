#include "magnetometer.h"

static volatile uint8_t ready = 0x00;
static double heading = 0;		//In radians

void magnetometer_init(){
	twi_init();
	uint8_t message[2];
	message[0] = 0x00;		//Register 0
	message[1] = 0x60;		//8 sample average, 0.75 Hz, normal mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x01;		//Register 1
	message[1] = 0x20;		//Default gain
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x02;		//Register 2
	message[1] = 0x00;		//Continuous measurement mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	
	PCMSK1 |= _BV(PCINT15);	//Enable PCINT 15 (SCK)
	PCICR |= _BV(PCIE1);	//Enable pin change interrupts for PCIE1
}

double magnetometer_read_heading(){
	if (ready){
		uint8_t message[4];
		message[0] = 0x04;	//Read 4 bytes; we don't care about Z for now.
		twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);
		
		int16_t x = message[0] << 8 | message[1];
		int16_t y = message[2] << 8 | message[3];
		
		if (message[0] & 0x08) x |= 0xF000;		//Negate x if needed
		if (message[2] & 0x08) y |= 0xF000;		//Negate y if needed

		heading = atan2(y, x);

		message[0] = 0x03;
		twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
	}
	return heading;
}

ISR(PCINT1_vect){
	if (PINB & _BV(PINB7)){
		ready = 0x01;
	}
}