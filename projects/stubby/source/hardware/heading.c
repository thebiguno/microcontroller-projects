#include "heading.h"
#include "status.h"

#define ALPHA 0.3

static volatile double filtered;

void heading_init(){
	magnetometer_init();
	filtered = magnetometer_read_heading();
	
	//Set up the timer to run at F_CPU / 1024, in normal mode
	TCCR2A = 0x0;
	TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
	TIMSK2 = _BV(TOIE2);
}

double heading_read(){
	return filtered;
}

void heading_take_reading(){
	double heading = magnetometer_read_heading();
	int8_t change = 0;
	
	if (filtered >= (M_PI / 2)){
		filtered -= M_PI;
		heading -= M_PI;
		change = 1;
	}
	else if (filtered <= (M_PI / -2)){
		filtered += M_PI;
		heading += M_PI;
		change = -1;
	}
	
	filtered = ALPHA * heading + (1 - ALPHA) * filtered;
	
	filtered += (M_PI * change);
}

EMPTY_INTERRUPT(TIMER2_COMPA_vect)
EMPTY_INTERRUPT(TIMER2_COMPB_vect)
ISR(TIMER2_OVF_vect){
	sei();
	static uint8_t i = 0;
	i++;
	if (i >= 10){
		i = 0;
		heading_take_reading();
		
		static uint8_t c = 0;
		c ^= 1;
		if (c){
			status_set_color(0xFF, 0x00, 0x00);
		}
		else {
			status_set_color(0x00, 0x00, 0xFF);
		}
	}
}
