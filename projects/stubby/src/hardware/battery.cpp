#include "battery.h"
#include <avr/interrupt.h>

#define ALPHA 0.2

uint8_t battery_status_enabled = 0x00;		//Should the battery levels show on status light?
volatile uint8_t battery_level = 0x00;		//Raw battery level; set in ADC ISR, cleared in status_set_battery_level()

//The value is the running average; initially we set this to 0x00 as a flag to take the first
// ADC value directly without averaging.
static uint8_t average = 0x00;


void battery_init(){
	//ADC Enable, ADC interrupt, prescaler F_CPU / 128
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	//Free running mode (the default, but in case it was set previously)
	ADCSRB = 0x00;

	//Set AREF mode: AREF = VCC, ADC Left Adjust, Pin A7
	ADMUX = _BV(REFS0) | _BV(ADLAR) | 0x07;
}

uint8_t battery_get_percent(){
	//By averaging here, we only include the last ADC value which was set prior to this call.
	// However, we don't want to do float math in the ISR, so this works out, I guess...
	if (average == 0){
		average = battery_level;
	}
	else {
		average = (uint8_t) (ALPHA * battery_level + (1 - ALPHA) * value);
	}

	if (average >= BATTERY_LEVEL_FULL) {
		return 100;
	}
	else if (average <= BATTERY_LEVEL_EMPTY){
		return 0;
	}
	else {
		return (uint8_t) (100.0 * (average - BATTERY_LEVEL_EMPTY) / (BATTERY_LEVEL_FULL - BATTERY_LEVEL_EMPTY));
	}
}

ISR(ADC_vect){
	battery_level = ADCH;
}
