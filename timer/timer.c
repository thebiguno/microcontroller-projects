#include "timer.h"

unsigned long long _timer_millis;
unsigned long _timer_micro_divisor;

/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs 
 * linked with timer1.
 */
void timer_init(){
	//Set up the timer to run at F_CPU
	TCCR1A = 0x0;
	TCCR1B |= (1 << CS10) | (1 << WGM12);
	
	//Set compare value to be F_CPU / 1000 (i.e. fire interrupt every millisecond)
	OCR1A = F_CPU / 1000;
	_timer_micro_divisor = F_CPU / 1000 / 1000;
	
	//Enable compare interrupt
	TIMSK1 = (1 << OCIE1A);

	//Reset count variables
	_timer_millis = 0l;
	
	//Enable interrupts
	sei();
}

/*
 * Returns the number of milliseconds which have elapsed since the 
 * last time timer_init() was called.  Overflows after about 49 days.
 */
unsigned long long timer_millis(){
	return _timer_millis;
}

/*
 * Returns the number of microseconds which have elapsed since the 
 * last time timer_init() was called.  Overflows after about 71 minutes.
 */
unsigned long long timer_micros(){
	return (_timer_millis * 1000) + (TCNT1 / _timer_micro_divisor);
}


/* 
 * The ISR for timer0 overflow.  Increment the _timer_count here, and do the calculcations
 * to increment _timer_millis as needed.
 */
ISR(TIMER1_COMPA_vect){
	_timer_millis++;
	
}
