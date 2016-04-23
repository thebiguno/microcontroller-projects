/*
 * Low resolution implementation of timer using a 8 bit clock with prescaler, 
 * etc.  Clicks approximately every millisecond.
 * You can use this file when you don't need high resolution timing, and don't care
 * about slight clock drift, inaccuracy, etc.  For timing most human-related activities,
 * though, it should be more than sufficient.
 */
#include "timer0.h"

//By using two counters (8 and 16 bit) we get: a) fast incrementing in ISR b) effective 24 bit resolution.  We don't 
// need more time than that (24bit ms == 16000 seconds == 279 minutes).  No single-battery flight time will be that long!
static volatile uint8_t _timer_millis;		//Resets every 256ms (overflow)
static volatile uint16_t _timer_millis_counter;		//Increments every 256ms
/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs 
 * linked with timer0.
 */
void timer0_init(){
	//Set up the timer to run at F_CPU / 256, in CTC mode
	TCCR0A = _BV(WGM01);
	TCCR0B = _BV(CS02);
	
	//Set compare value to be F_CPU / 1000 -- fire interrupt every millisecond
	OCR0A = F_CPU / 256 / 1000;
	
	//Enable compare interrupt
	TIMSK0 = _BV(OCIE0A);

	//Reset count variables
	_timer_millis = 0;
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
	sei();
}

/*
 * Returns the number of milliseconds which have elapsed since the 
 * last time timer_init() was called.  Overflows after about 49 days.
 */
uint32_t timer_millis(){
	return (_timer_millis_counter << 8) + _timer_millis;
}

/* 
 * The ISR for timer0 overflow.  Increment the _timer_count here, and do the calculcations
 * to increment _timer_millis as needed.
 */
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)
ISR(TIMER0_COMPA_vect){
	_timer_millis++;
	if (_timer_millis == 0){
		_timer_millis_counter++;
	}
}
