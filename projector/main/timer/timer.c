#include "timer.h"
#include "../button/button.h"
#include "../shift/shift.h"

#define COMPA (F_CPU / 256 / 1000)

static volatile uint32_t _timer_millis;

void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (TCNT0 is reset in the ISR)
	TCCR0A = 0x0;			// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR0B |= _BV(CS02);	// clock select = CLK / 256 (prescaler)
	
	OCR0A = COMPA;			// interrupt A = every millisecond
	
	TIMSK0 = _BV(OCIE0A) | _BV(OCIE0B);

#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
	_timer_millis = 0;
}

uint32_t timer_millis() {
	return _timer_millis;
}

ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0;						// reset counter to zero
	_timer_millis++;
}
