#include "timer.h"

#define COMPA (F_CPU / 1 / 1000)

// every 4 millis reset every second 
static volatile uint16_t _timer_millis;
// seconds since epoch
static volatile uint32_t _timer_seconds;

void timer_init() {
	//Set up the timer to run at F_CPU / 256, in normal mode (TCNT0 is reset in the ISR)
	TCCR1A = 0x0;			// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR1B |= _BV(CS10);	// clock select = CLK / 1 (no prescaler)
	
	timer_set_tune(-12);
	
	TIMSK1 = _BV(OCIE1A);

#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
	_timer_seconds = 0;
}

int16_t timer_get_tune() {
	return OCR1A - COMPA;
}
void timer_set_tune(int16_t tune) {
	OCR1A = COMPA + tune;
}

uint32_t timer_get_seconds() {
	return _timer_seconds;
}

uint16_t timer_get_millis() {
	return _timer_millis;
}

void timer_set(uint32_t seconds, uint16_t millis) {
	cli();
	_timer_seconds = seconds;
	_timer_millis = 0;
	_timer_millis = millis;
	TCNT1 = 0;
	sei();
}

ISR(TIMER1_COMPA_vect) {
	TCNT1 = 0;						// reset counter to zero
	_timer_millis++;
	
	if (_timer_millis >= 1000) {
		_timer_millis = 0;
		_timer_seconds++;
	}
}

