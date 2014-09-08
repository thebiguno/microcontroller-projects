#include "status.h"
#include "Stubby.h"
#include "lib/pwm/pwm.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>

void status_init(){
	//Set up the timer to run in normal mode
	TCCR0A = 0x0;
	//FCPU / 1024 prescaler
	TCCR0B = _BV(CS02) | _BV(CS00);
	
	//Enable timer overflow interrupts
	TIMSK0 = _BV(TOIE0);


	//ADC Enable, ADC interrupt, prescaler = 0x7 (F_CPU / 128)
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | 0x07;

	//Free running mode (already default, but in case it was set previously)
	ADCSRB = 0x00;
	
	//Set AREF mode: AREF = VCC, ADC Left Adjust, Pin A7
	ADMUX = _BV(REFS0) | _BV(ADLAR) | 0x07;
	
}

void status_enable_battery(){
	//Set timer to run at F_CPU / 1024
	TCCR0B |= _BV(CS02) | _BV(CS00);
}

void status_disable_battery(){
	//Disable timer
	TCCR0B = 0x00;
}

void status_flash(uint8_t red, uint8_t green, uint8_t blue, uint8_t count){
	for (uint8_t i = 0; i < count; i++){
		wdt_reset();
		status_set_color(red, green, blue);
		_delay_ms(100);
		status_set_color(0x00, 0x00, 0x00);
		_delay_ms(100);
	}	
}

void status_set_color(uint8_t red, uint8_t green, uint8_t blue){
	pwm_set_phase_batch(LED_RED, ((uint16_t) red) * PHASE_MULTIPLIER);
	pwm_set_phase_batch(LED_GREEN, ((uint16_t) green) * PHASE_MULTIPLIER);
	pwm_set_phase_batch(LED_BLUE, ((uint16_t) blue) * PHASE_MULTIPLIER);
	
	pwm_apply_batch();
}

/* 
 * The ISR for timer0 overflow.
 */
EMPTY_INTERRUPT(TIMER0_COMPA_vect)
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
ISR(TIMER0_OVF_vect){
	//PWM should always be running
	sei();
	
	static uint8_t count = 0x00;
	count++;
	if (count >= 0x0F){
		//Start ADC
		ADCSRA |= _BV(ADSC);
		
		count = 0x00;
	}
}

ISR(ADC_vect){
	//Keep PWM signals going
	sei();
	
	//The value is the running average; initially we set this to 0x00 as a flag to take the first
	// ADC value directly without averaging.
	static uint8_t value = 0x00;
	
	uint8_t new_value = ADCH;
	
	if (value == 0x00){
		value = new_value;
	}
	else {
		value = (15 * (uint16_t) value + new_value) >> 4;
	}
	
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
}
