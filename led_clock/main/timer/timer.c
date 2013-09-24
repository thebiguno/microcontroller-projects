#include "timer.h"

#define COMPA (F_CPU / 256 / 1000) - 1

static volatile uint32_t _timer_millis;

void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (TCNT0 is reset in the ISR)
	TCCR0A = 0x0;			// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR0B |= _BV(CS02);	// clock select = CLK / 256 (prescaler)
	
	OCR0A = COMPA;		// interrupt A = every millisecond
	
	TIMSK0 = _BV(OCIE0A);

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

ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0;						// reset counter to zero
	_timer_millis++;
	
	if (_timer_millis > 86400000) _timer_millis = 0;
}

