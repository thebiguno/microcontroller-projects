#include "analog.h"

//This is a 3 bit value; refer to the data sheet for what each mask means.  A 
// reasonable default seems to be 0x4 (/16) or 0x5 (/32).  Higher prescalers
// will give more accurate results, at the cost of longer conversion times.
#ifndef ADC_PRESCALER_MASK
#define ADC_PRESCALER_MASK 0x4
#endif

static uint8_t pins[8];
static uint8_t pin_count;

void analog_init(uint8_t analog_pins[], uint8_t count, uint8_t aref){
	pin_count = count;

	for (int i = 0; i < count; i++){
		//Store which pins are needed
		pins[i] = analog_pins[i];

		//Disable digital input on selected pins.
		//TODO Should we do this?
		DIDR0 |= _BV(pins[i]);
	}

	//ADC Enable, prescaler as specified with default of 0x4 (F_CPU / 16)
	ADCSRA |= _BV(ADEN) | ADC_PRESCALER_MASK;

	//Set AREF mode.
	//TODO Add support for REFS2, used on ATtiny85 (and presumably more)...
	ADMUX &= ~(_BV(REFS0 + 1) | _BV(REFS0));  //Clear AREF bits...
	ADMUX |= aref << REFS0; //... and set according to aref
	
	//Free running mode (already default, but in case it was set previously)
	ADCSRB = 0x00;
}

void analog_read_a(uint16_t *data){
	for (int i = 0; i < pin_count; i++){
		data[i] = analog_read_p(i);
	}
}

uint16_t analog_read_p(uint8_t index){
	//Set up which pin to read
	ADMUX = (ADMUX & 0xF0) | pins[index];

	//Start conversion	
	ADCSRA |= _BV(ADSC);
	
	//Wait until conversion is complete
	while (ADCSRA & _BV(ADSC));
	
	//Return conversion result
	return ADC;
}

