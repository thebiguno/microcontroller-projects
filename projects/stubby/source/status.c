#include "status.h"

void status_init(){
	//Set up the timer to run at F_CPU / 1024, in normal mode (we reset TCNT0 in the ISR)
	TCCR0A = 0x0;
	TCCR0B |= _BV(CS02) | _BV(CS00);
	
	TIMSK0 = _BV(TOIE0);	//Enable timer overflow interrupts
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void status_set_color(uint8_t red, uint8_t green, uint8_t blue){
	//The magic number 79 comes from X/20000 = color/255; X = color * 20000 / 255; X = color * 79.
	pwm_set_phase_batch(LED_RED, ((uint16_t) red) * 79);
	pwm_set_phase_batch(LED_GREEN, ((uint16_t) green) * 79);
	pwm_set_phase_batch(LED_BLUE, ((uint16_t) blue) * 79);
	
	pwm_apply_batch();
}

/* 
 * The ISR for timer0 overflow.
 */
EMPTY_INTERRUPT(TIMER0_COMPA_vect)
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
ISR(TIMER0_OVF_vect){
	sei();
	
	static uint8_t counter = 0;
	counter++;
	if (counter >= 16){		//Only do stuff when counter reaches a given value; this is effectively a further prescaler.
		counter = 0;
		static uint8_t red = 0;
		status_set_color(red++, 0, 0);
	}
}

