#ifndef TIMER_HAL_H
#define TIMER_HAL_H

#include "stm32f4xx_hal.h"

#if defined (__cplusplus)
extern "C" {
#endif

#ifndef TIMER_BITS
#define TIMER_BITS 64
#endif

/*
 * Initializes the timer, and resets the timer count to 0.
 */
void timer_init();

/*
 * Returns the number of milliseconds which have elapsed since the 
 * last time timer_init() was called.  Overflows after about 49 days (32 bits)
 * or a whole lot longer (64 bits).
 */
#if TIMER_BITS == 64
uint64_t timer_millis();
#elif TIMER_BITS == 32
uint32_t timer_millis();
#endif

#if defined (__cplusplus)
}
#endif

#endif
