#ifndef PWM_H
#define PWM_H

#include <avr/io.h>

/*
 * Initializes the library to use the specified port and pin for PWM output, and turns on
 * the PWM timers at the given duty cycle.  To turn off, pass a duty cycle of 0.  Max duty 
 * cycle is 0xFF (always on).
 * 
 * You need to pass the ports by reference - e.g. pwm_init(&PORTC, PORTC0, 0x7F).
 * 
 * Note that depending on the AVR chip you are using, you a) may not have any PWM outputs
 * available, b) have only a few pins available.  This function will verify that the
 * desired port / pin is valid for PWM; if it is not valid, it will return 0xFF.  On 
 * success this will return 0x00.  You should check the return value to verify that 
 * the function call has worked.
 */
uint8_t pwm_set(volatile uint8_t *port, volatile uint8_t *ddr, uint8_t pin, uint8_t duty_cycle);

#endif
