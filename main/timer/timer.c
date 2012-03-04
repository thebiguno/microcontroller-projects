#include "timer.h"
#include "../button/button.h"
#include "../shift/shift.h"

static volatile uint32_t _timer_millis;

void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (we reset TCNT0 in the ISR)
	TCCR0A = 0x0;			// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR0B |= _BV(CS02);	// clock select = F_CPU / 256
	
	OCR0A = F_CPU / 256 / 1000;		// interrupt A = every millisecond
	// OCR0B = F_CPU / 256 / 4000;		// interrupt B = every 1/4 millisecond
	
	TIMSK0 = _BV(OCIE0A);

#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
//	DDRB = 0xFF;
}

uint64_t timer_millis() {
	return _timer_millis;
}

void timer_add(uint32_t millis) {
	_timer_millis += millis;
}

EMPTY_INTERRUPT(TIMER0_OVF_vect)
EMPTY_INTERRUPT(TIMER0_COMPB_vect)

ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0;						// reset counter to zero
	_timer_millis++;
	
	if (_timer_millis > 86400000) _timer_millis = 0;
}

// ISR(TIMER0_COMPB_vect) {
// 	OCR0B += F_CPU / 256 / 4000;	// increment compare value
// }
