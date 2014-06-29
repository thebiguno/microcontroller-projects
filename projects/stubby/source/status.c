#include "status.h"
#include "Stubby.h"
#include "lib/pwm/pwm.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>

void status_init(){
	//Set up the timer to in normal mode
	TCCR0A = 0x0;
	TCCR0B |= _BV(CS02) | _BV(CS00);
	
	TIMSK0 = _BV(TOIE0);	//Enable timer overflow interrupts


	
	//Set AREF mode: AREF = VCC, ADC Left Adjust, Pin A7
	ADMUX = _BV(REFS0) | _BV(ADLAR) | 0x07;

	//Free running mode (already default, but in case it was set previously)
	ADCSRB = 0x00;
	
	//ADC Enable, ADC interrupt, prescaler = 0x7 (F_CPU / 128)
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | 0x07;
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
	//The magic number 78 comes from X/20000 = color/255; X = color * 20000 / 255; X = color * 78.
	pwm_set_phase_batch(LED_RED, ((uint16_t) red) * 78);
	pwm_set_phase_batch(LED_GREEN, ((uint16_t) green) * 78);
	pwm_set_phase_batch(LED_BLUE, ((uint16_t) blue) * 78);
	
	pwm_apply_batch();
}

/* 
 * The ISR for timer0 overflow.
 */
EMPTY_INTERRUPT(TIMER0_COMPA_vect)
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
ISR(TIMER0_OVF_vect){
	//Start ADC
	ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect){
	//Keep PWM signals going
	sei();
	
	//The value is the running average
	static uint8_t value = 1;
	
	uint8_t new_value = ADCH;
	
	value = (new_value + (uint16_t) value * 0xFF) >> 8;
	
	pwm_set_phase_batch(LED_RED, ((uint16_t) 200 - value) * 78);
	pwm_set_phase_batch(LED_GREEN, ((uint16_t) value - 100) * 78);
}
