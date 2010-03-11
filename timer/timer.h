#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs 
 * linked with timer1.
 */
void timer_init();

/*
 * Returns the number of milliseconds which have elapsed since the 
 * last time timer_init() was called.  Overflows after about 49 days.
 */
unsigned long long timer_millis();

/*
 * Returns the number of microseconds which have elapsed since the 
 * last time timer_init() was called.  Overflows after about 71 minutes.
 */
unsigned long long timer_micros();
