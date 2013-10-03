#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs 
 * linked with timer1.
 */
void timer_init();

void timer_set(uint32_t millis);
void timer_set_tune(int8_t millis);
int8_t timer_get_tune();

/*
 * Returns the number of milliseconds which have elapsed since the 
 * last time timer_init() was called.  Overflows after about 49 days.
 */
uint32_t timer_millis();

#endif
