/*
 * Low resolution implementation of timer using a 8 bit clock with prescaler, 
 * etc.  Clicks approximately every millisecond.  Sub-millisecond resolution is available
 * on demand, and has approximately 12µs resolution at 20MHz.
 * You can use this file when you don't need high resolution timing, and don't care
 * about slight clock drift, inaccuracy, etc.  For timing most human-related activities,
 * though, it should be more than sufficient.
 */
#include "TimerHAL.h"

#if TIMER_BITS == 64
static volatile uint64_t millis;
#elif TIMER_BITS == 32
static volatile uint32_t millis;
#endif

/*
 * Initializes the timer
 */
void timer_init(){
	//Reset count variables
	millis = 0;
}

#if TIMER_BITS == 64
uint64_t timer_millis(){
#elif TIMER_BITS == 32
uint32_t timer_millis(){
#endif
	return millis;
}


/* 
 * The ISR callback (from HAL).  Increment millis here
 */
void HAL_SYSTICK_Callback(void){
	millis++;
}