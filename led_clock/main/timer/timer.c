#include "timer.h"

#define COMPA (F_CPU / 1 / 1000 / 4) - 12

// every 4 millis reset every second 
static volatile uint8_t _timer_millis;
// seconds in the day
static volatile uint16_t _timer_seconds;

void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (TCNT0 is reset in the ISR)
	TCCR1A = 0x0;			// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR1B |= _BV(CS10);	// clock select = CLK / 1 (no prescaler)
	
	OCR1A = COMPA;		// interrupt A = every millisecond
	
	TIMSK1 = _BV(OCIE1A);

#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
	_timer_seconds = ((uint32_t) 12 * 60 * 60) + ((uint32_t) 01 * 60); //0;
}

uint16_t timer_seconds() {
	return _timer_seconds;
}
uint32_t timer_millis() {
	return _timer_seconds * 1000 + _timer_millis * 4;
}

void timer_add(uint32_t millis) {
	_timer_millis += millis;
}

ISR(TIMER1_COMPA_vect) {
	TCNT1 = 0;						// reset counter to zero
	_timer_millis++;
	
	if (_timer_millis > 250) {
		_timer_millis = 0;
		_timer_seconds++;
	}
}

