/*
 * PWM library which allows you to control an arbitrary number of outputs.  All
 * outputs must have the same period, but each one can have a different phase.
 * This library relies on a 16 bit TIMER1, so cannot be run on any chip which does
 * not have access to this hardware.
 *
 * Please set CDEF PWM_PORTx_UNUSED for all ports which either are not on your chip,
 * or which are not being used for PWM.  (If your chip does not have a given port, e.g.
 * the ATMega168 does not have a PORTA, then there will be compile errors.  If you
 * have the hardware, but are not using it for PWM, then it will just use (slightly) more
 * memory and will be (slightly) slower.
 * 
 * Written by Wyatt Olson and Warren Janssens
 * http://digitalcave.ca
 * Released under the Creative Commons Attribution-NonCommercial-ShareAlike license.
 */

#ifndef PWM_H
#define PWM_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>

//The absolute maximum number of pins defined.  The actual 'used' count is passed
// into pwm_init, and there is no problem with having a larger number here than is
// actually used (other than a bit of wasted SRAM space for pre-defined arrays).
// By default we set this to 4 as that is a decently large number, but not so huge
// that tons of excess SRAM is wasted.  If you are low on memory, or need more
// outputs, feel free to redefine this in your makefile (in the CDEFS variable,
// beside where F_CPU is defined).
#ifndef PWM_MAX_PINS
#define PWM_MAX_PINS 4
#endif


/*
 * Initializes the PWM library at a given period, using the specified ports.
 * You can have up to PWM_MAX_PINS pins defined here. 
 * 
 * To initialize this, you must pass in references to each of the PORTs, DDRs, and
 * the values of each pin.  You can do it like this:
 * 
 *    volatile uint8_t *ports[8];
 *    ports[0] = &PORTB;
 *    ports[1] = &PORTD;
 *    ....
 *    ports[5] = &PORTD;
 *
 *    uint8_t pins[8];
 *    pins[0] = 1;
 *    ....
 *    pins[5] = 3;
 *
 *    pwm_init(ports, pins, 6, 20000);
 *
 * While the code to do this is uglier than if it were to just be a function 
 * call, by allowing an arbitrary number of PWM pins, it is best to do it like this.
 *
 * The period must be less than 3354624µs (about 3.3 seconds); any more than this and
 * we just set it to the maximum.  There is no theoretical minimum, but in practice
 * you should probably keep it at 1000µs or so (1ms).
 */
void pwm_init(volatile uint8_t *ports[],
				uint8_t pins[],
				uint8_t count,
				uint32_t period);
				
				
/*
 * Sets the phase of the pin at the given index to the specified value.  The index
 * matches the index used for the arrays in pwm_init().
 * Internally, this calls pwm_set_phase_batch() and then pwm_apply_batch().
 */
void pwm_set_phase(uint8_t index, uint32_t phase);

/*
 * Sets the phase of the pin at the given index to the specified value, but does not
 * apply the change (recalculate all pin values).  This can be called multiple times
 * for different pins with minimal overhead; when completed all changes, call pwm_apply_batch().
 */
void pwm_set_phase_batch(uint8_t index, uint32_t phase);

/*
 * Recalculates all pin / timer values.  Required after calling pwm_set_phase_batch().
 */
void pwm_apply_batch();

/*
 * Sets the period of the PWM generator, changing the period specified in init().  
 * Applies to all active PWM pins.
 */
void pwm_set_period(uint32_t period);

/*
 * Turns off the PWM generator and stops the timer clock.  Does not clear any config
 * variables.  You can turn PWM back on again using the pwm_start() function.
 */
void pwm_stop();

/*
 * Turns on the PWM generator, starting back at 0.  This should be called after pwm_off() 
 * has been called, but pwm_init() *must* already have been called.  Calling this when
 * the generator is already on will reset the timer to 0, but otherwise will have no effect.
 *
 * If TIMER1 control registers have been modified by other libraries, you must call 
 * pwm_init() instead of pwm_start().
 */
void pwm_start();

#endif
