#include "distance.h"

volatile uint8_t interval_do_distance_reading = 0x00;		//Mailbox value, set in timer2 ISR and read in delay

#define TEN_MICROSECONDS (F_CPU / 100000)
#define ALPHA 0.3

#if DISTANCE_SENSOR == 1
static int16_t filtered;

void distance_measure(){
	//Bring trigger high for more than 10uS.  20uS seems to work fine.
	TRIGGER_PORT |= _BV(TRIGGER_PIN);
	_delay_us(20);
	TRIGGER_PORT &= ~_BV(TRIGGER_PIN);
	
	//Set up timer0 to run at F_CPU (no prescaler), in normal mode
	TCCR0A = 0x0;
	TCCR0B = _BV(CS00);

	//Determine how long the data pin is high.  Wait until it goes high, then start timing.
	while ((ECHO_PORT & _BV(ECHO_PIN)) == 0x00);
	TCNT0 = 0;
	uint16_t micros = 0;
	
	//Delay until C4 goes low
	while (ECHO_PORT & _BV(ECHO_PIN)){
		if (TCNT0 > TEN_MICROSECONDS) {
			TCNT0 = 0x00;
			micros += 10;
		}
	}
	
	//Turn off the timer
	TCCR0B = 0x00;
	
	//Get any partial time remaining
	micros += ((TCNT0 * (uint16_t) 10) / TEN_MICROSECONDS);
	
	//Calculate distance; 5.8 is a magic number calculated from the speed of sound.
	uint16_t distance = (uint16_t) (micros / 5.8);
	
	//Add to the running average
	filtered = ALPHA * distance + (1 - ALPHA) * filtered;
}

uint16_t distance_read(){
	return filtered;
}

#else

void distance_measure();

uint16_t distance_read(){ return 0; }

#endif