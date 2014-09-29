#include "timer2.h"

extern volatile uint8_t interval_do_magnetometer_reading;	//Defined in magnetometer.c

EMPTY_INTERRUPT(TIMER2_COMPA_vect)
EMPTY_INTERRUPT(TIMER2_COMPB_vect)
ISR(TIMER2_OVF_vect){
	sei();
	static uint8_t i = 0;
	i++;
	
	if ((i & 0x07) == 0x00){
		interval_do_magnetometer_reading = 0x01;
	}
	if ((i & 0x0F) == 0x00){
		ADCSRA |= _BV(ADSC);		//Start ADC
	}
}
