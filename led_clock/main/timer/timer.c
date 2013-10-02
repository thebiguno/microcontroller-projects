#include "timer.h"

#define COMPA (F_CPU / 1 / 1000) - 48

static volatile uint32_t _timer_millis;

void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (TCNT0 is reset in the ISR)
	TCCR1A = 0x0;			// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR1B |= _BV(CS10);	// clock select = CLK / 1 (no prescaler)
	
	OCR1A = COMPA;		// interrupt A = every millisecond
	
	TIMSK1 = _BV(OCIE1A);

#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
	_timer_millis = ((uint32_t) 12 * 60 * 60 * 1000) + ((uint32_t) 01 * 60 * 1000); //0;
}

uint64_t timer_millis() {
	return _timer_millis;
}

void timer_add(uint32_t millis) {
	_timer_millis += millis;
}

ISR(TIMER1_COMPA_vect) {
	TCNT1 = 0;						// reset counter to zero
	_timer_millis++;
	
	if (_timer_millis > 86400000) _timer_millis = 0;
}

