#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs 
 * linked with timer1.
 */
void timer_init();

/*
 * Returns the number of milliseconds which have elapsed since the 
 * last time timer_init() was called.
 */
uint32_t timer_millis();

#endif
