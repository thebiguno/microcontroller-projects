#include "analog.h"

/*
 * To enable this library, you need to add -DADC_HARDWARE=2 to the CDEFS
 */
#if defined(ADC_HARDWARE) && ADC_HARDWARE == 2

#include <avr/interrupt.h>

//This is a 3 bit value; refer to the data sheet for what each mask means.  A
// reasonable default seems to be 0x4 (/16) or 0x5 (/32).  Higher prescalers
// will give more accurate results, at the cost of longer conversion times.
//Note that a beneficial side-effect of larger ADC values is that the ISR will
// run less frequently; if you want the ADC code to have less of an impact on
// other runnng code, then you can increase this value to 0x7 (/128).
#ifndef ADC_PRESCALER_MASK
#define ADC_PRESCALER_MASK 0x4
#endif

//The maximum number of pins allowed.  Defaults to 8.  If you need more, set
// this value in CDEFS.  If you want to save a few bytes of memory, you can
// set this to be exactly the size you need.
#ifndef ADC_MAX_PINS
#define ADC_MAX_PINS 8
#endif

static uint8_t pins[ADC_MAX_PINS];
static uint8_t pin_count;

//Buffer for result values
static volatile uint16_t results[ADC_MAX_PINS];

void analog_init(uint8_t analog_pins[], uint8_t count, uint8_t aref){
	pin_count = count;

	for (int i = 0; i < count; i++){
		//Store which pins are needed
		pins[i] = analog_pins[i];

		//Disable digital input on selected pins.
		//TODO Should we do this?
		DIDR0 |= _BV(pins[i]);
	}

	//ADC Enable, ADC interrupt, prescaler = 0x4 (F_CPU / 16)
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | ADC_PRESCALER_MASK;

	//Set AREF mode.
	//TODO Add support for REFS2, used on ATtiny85 (and presumably more)...
//	if (aref != 0x1 && aref != 0x3) aref = 0x0; //Sanity check...
	ADMUX &= ~(_BV(REFS0 + 1) | _BV(REFS0));  //Clear AREF bits...
	ADMUX |= aref << REFS0; //... and set according to aref

	//Free running mode (already default, but in case it was set previously)
	ADCSRB = 0x00;

	//Start ADC conversions
	ADCSRA |= _BV(ADSC);

	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void analog_read_a(uint16_t *data){
	for (int i = 0; i < pin_count; i++){
		data[i] = results[i];
	}
}

uint16_t analog_read_p(uint8_t index){
	//Return last conversion result
	return results[index];
}

//Counter for the ISR to know which ADC it is talking to at a given time.  Used to
// configure the ADC mux channel section.  This value refers to the index of
// the pins variable.
static volatile uint8_t pin_index = 0;

ISR(ADC_vect){
	//Read last ADC value assuming calibration is finished
	results[pin_index] = ADC;

	//Increment to next pin and start again
	pin_index = (pin_index + 1) % pin_count;

	if (pins[pin_index] <= 0x07){
		ADMUX = ((ADMUX) & 0xE0) | pins[pin_index];
		ADCSRB &= ~_BV(MUX5);
	}
	else {
		ADMUX = ((ADMUX) & 0xE0) | (pins[pin_index] - 0x08);
		ADCSRB |= _BV(MUX5);
	}

	//Start ADC again
	ADCSRA |= _BV(ADSC);
}

#endif
