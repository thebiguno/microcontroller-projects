#include "timer.h"
#include "../button/button.h"
#include "../shift/shift.h"

static volatile uint32_t _timer_millis;

void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (TCNT0 is reset in the ISR)
	TCCR0A = 0x0;			// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR0B |= _BV(CS02);	// clock select = CLK / 256 (prescaler)
	
	OCR0A = F_CPU / 256 / 1000;		// interrupt A = every millisecond
	OCR0B = 1;		// interrupt B = 
	
	TIMSK0 = _BV(OCIE0A) | _BV(OCIE0B);

#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
	_timer_millis = 0;
}

uint64_t timer_millis() {
	return _timer_millis;
}

void timer_add(uint32_t millis) {
	_timer_millis += millis;
}

EMPTY_INTERRUPT(TIMER0_OVF_vect)
//EMPTY_INTERRUPT(TIMER0_COMPB_vect)

ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0;						// reset counter to zero
	OCR0B = 1;						// reset other compare value to zero
	_timer_millis++;
	
	if (_timer_millis > 86400000) _timer_millis = 0;
	
	button_sample();
}

ISR(TIMER0_COMPB_vect, ISR_NOBLOCK) {
	OCR0B += 1;	// increment compare value
	shift_do();
}
