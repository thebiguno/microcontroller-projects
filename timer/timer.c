#include "timer.h"

unsigned long long _timer_millis;
unsigned long _timer_millis_count;
unsigned long _timer_millis_count_overflow;

unsigned long long _timer_micros;
unsigned long _timer_micros_count;
unsigned long _timer_micros_count_overflow;

/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs 
 * linked with timer0.
 */
void timer_init(){
	//Set up the timer to run at F_CPU / 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	
	//Enable Overflow Interrupt Enable
	TIMSK0 |= (1 << TOIE0);

	//Reset count variables
	_timer_millis = 0l;
	_timer_millis_count = 0l;
	
	//Enable interrupts
	sei();

	//Calculate the point at which _timer_count overflows, based on CPU speed.
	// The 8 comes from the 8x prescaler.  The 256 comes from the 8 bit timer 
	// register; it overflows every F_CPU / 256 cycles.  
	_timer_micros_count_overflow = F_CPU / 64 / 256;
	
	//The 1000 is micro to millis; F_CPU is in Hz, so F_CPU / 1000 is cycles / millisecond.
	_timer_millis_count_overflow = _timer_micros_count_overflow / 1000;

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
	return _timer_micros;
}


/* 
 * The ISR for timer0 overflow.  Increment the _timer_count here, and do the calculcations
 * to increment _timer_millis as needed.
 */
ISR(TIMER0_OVF_vect){
	_timer_millis_count++;
	_timer_micros_count++;
	if (_timer_millis_count >= _timer_millis_count_overflow){
		_timer_millis++;
		_timer_millis_count = 0;
	}
	if (_timer_micros_count >= _timer_micros_count_overflow){
		_timer_micros++;
		_timer_micros_count = 0;
	}
}
