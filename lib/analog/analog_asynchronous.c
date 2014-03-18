#include "analog.h"

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

static uint8_t pins[8];
static uint8_t pin_count;

//Buffer for result values
static uint16_t results[8];

//Counter for the ISR to know which ADC it is talking to at a given time.  Used to 
// configure the ADC mux channel section.  This value refers to the index of
// the pins variable.
static uint8_t adc_mux = 0;

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

ISR(ADC_vect){
	//Read last ADC value assuming calibration is finished
	results[adc_mux] = ADC;

	//Increment to next pin and start again
	adc_mux = (adc_mux + 1) % pin_count;
	ADMUX = ((ADMUX) & 0xF0) | pins[adc_mux];

	//Start ADC again
	ADCSRA |= _BV(ADSC);
}
