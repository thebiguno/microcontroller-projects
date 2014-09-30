#include "battery.h"

#define ALPHA 0.2

uint8_t battery_status_enabled = 0x00;		//Should the battery levels show on status light?
volatile uint8_t battery_level = 0x00;		//Raw battery level; set in ADC ISR, cleared in status_set_battery_level()

//The value is the running average; initially we set this to 0x00 as a flag to take the first
// ADC value directly without averaging.
static uint8_t value = 0x00;


void battery_init(){
	//ADC Enable, ADC interrupt, prescaler F_CPU / 128
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	//Free running mode (the default, but in case it was set previously)
	ADCSRB = 0x00;
	
	//Set AREF mode: AREF = VCC, ADC Left Adjust, Pin A7
	ADMUX = _BV(REFS0) | _BV(ADLAR) | 0x07;
}

void battery_enable_status(){
	battery_status_enabled = 0x01;
}

void battery_disable_status(){
	battery_status_enabled = 0x00;
}

void battery_set_level(){
	if (value == 0x00){
		value = battery_level;
	}
	else {
		value = (uint8_t) (ALPHA * battery_level + (1 - ALPHA) * value);
	}
	
	if (battery_status_enabled){
		pwm_set_phase_batch(LED_BLUE, 0x00);
	
		if (value >= BATTERY_LEVEL_FULL){
			pwm_set_phase_batch(LED_RED, 0x00 * PHASE_MULTIPLIER);
			pwm_set_phase_batch(LED_GREEN, 0xFF * PHASE_MULTIPLIER);
		}
		else if (value <= BATTERY_LEVEL_EMPTY){
			pwm_set_phase_batch(LED_RED, 0xFF * PHASE_MULTIPLIER);
			pwm_set_phase_batch(LED_GREEN, 0x00 * PHASE_MULTIPLIER);
		}
		else {
			pwm_set_phase_batch(LED_RED, (BATTERY_LEVEL_FULL - value) * BATTERY_STATUS_MULTIPLIER);
			pwm_set_phase_batch(LED_GREEN, (value - BATTERY_LEVEL_EMPTY) * BATTERY_STATUS_MULTIPLIER);
		}
		
		pwm_apply_batch();
	}

	battery_level = 0x00;
}

ISR(ADC_vect){
	battery_level = ADCH;
}
