#include "analog.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>

//This is a 3 bit value; refer to the data sheet for what each mask means.  A 
// reasonable default seems to be 0x4 (/16) or 0x5 (/32).  Higher prescalers
// will give more accurate results, at the cost of longer conversion times.
#ifndef ADC_PRESCALER_MASK
#define ADC_PRESCALER_MASK 0x4
#endif

//If defined, we will not use the ADC sleep mode to reduce noise, and instead
// will busy-wait for conversion to complete.  For synchronous analog reads, 
// ADC sleep mode makes sense for most applications, unless there are other critical
// systems which are running in interrupts which may be interfered with by sleeping.
//Only define this if you have a good reason to.
//#define ADC_NO_SLEEP_MODE

/*
 * This is a synchronous implementation to read the ADC (synchronous meaning that when you 
 * call analog_read_p() it will start a new reading and not return until the reading is
 * completed).  Since we block on read anyway, we also use the sleep mode ADC noise cancelling
 * option to make for a better quality signal.
 */

static uint8_t pins[8];
static uint8_t pin_count;

void analog_init(uint8_t analog_pins[], uint8_t count, uint8_t aref){
	pin_count = count;

	for (int i = 0; i < count; i++){
		//Store which pins are needed
		pins[i] = (analog_pins[i] & 0x0F);

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

#ifdef ADC_NO_SLEEP_MODE
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
#else
uint16_t analog_read_p(uint8_t index){
	//Set up which pin to read
	ADMUX = (ADMUX & 0xF0) | pins[index];

	ADCSRA |= _BV( ADIE ); 	//Set ADC Interrupt
	set_sleep_mode(SLEEP_MODE_ADC); 
	// Enter Sleep Mode To Trigger ADC Measurement 
	// CPU Will Wake Up From ADC Interrupt
	sleep_enable();
	sei();		//Ensure interrupts are enabled, or else CPU will hang until hard reset
	sleep_cpu();
	sleep_disable();
	
	//Confirm that conversion is complete - this is generally not needed,
	// but the AVR may wake up from another interrupt source before 
	// conversion is done, and we want to ensure that we send the correct data.
//	while (ADCSRA & _BV(ADSC));
	
	//Return conversion result
	return ADC;
}

// ADC Interrupt Is Used To Wake Up CPU From Sleep Mode 
EMPTY_INTERRUPT (ADC_vect)
#endif