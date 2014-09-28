#include "magnetometer.h"

#define MAGNETOMETER_ADDRESS 0x1E
#define ALPHA 0.2

#if MAGNETOMETER == 1
static int16_t filtered[2];
static int16_t offsets[2];

void magnetometer_init(){
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
	_delay_ms(100);
	message[0] = 0x06;
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);

	//Read an actual value
	message[0] = 0x03;
	twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
	_delay_ms(100);
	message[0] = 0x06;
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);

	//Initialize the filtered values
	filtered[0] = (((uint16_t) message[0]) << 8 | message[1]) + offsets[0];
	filtered[1] = (((uint16_t) message[4]) << 8 | message[5]) + offsets[1];
	
	//Set up the timer to run at F_CPU / 1024, in normal mode
	TCCR2A = 0x0;
	TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
	TIMSK2 = _BV(TOIE2);
}

void magnetometer_set_offsets(int16_t x, int16_t y){
	offsets[0] = x;
	offsets[1] = y;
}

void magnetometer_take_reading(){
	//Read 6 bytes starting at X MSB (0x03, reset to here prior to this invocation)
	uint8_t message[6];
	message[0] = 0x06;
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);
	
	int16_t x = (((uint16_t) message[0]) << 8 | message[1]) + offsets[0];
	int16_t y = (((uint16_t) message[4]) << 8 | message[5]) + offsets[1];
	
	//From what I have seen, spurious readings (saturation?) seem to show up as very large values
	// in the thousands... most normal readings seem to be in the -250..250 range.  By filtering
	// out any readings outside of |500| we should eliminate most (all?) of the spurious readings.
	if (x >= -500 && x <= 500 && y >= -500 && y <= 500){
		filtered[0] = ALPHA * x + (1 - ALPHA) * filtered[0];
		filtered[1] = ALPHA * y + (1 - ALPHA) * filtered[1];
	}

	//Reset pointer to read next time
	message[0] = 0x03;
	twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
}

double magnetometer_read_heading(){
	return atan2(filtered[1], filtered[0]);
}

EMPTY_INTERRUPT(TIMER2_COMPA_vect)
EMPTY_INTERRUPT(TIMER2_COMPB_vect)
ISR(TIMER2_OVF_vect){
	sei();
	static uint8_t i = 0;
	if (i++ >= 5){
		i = 0;
		magnetometer_take_reading();
	}
}
#else
double magnetometer_read_heading(){
	return 0;
}
#endif