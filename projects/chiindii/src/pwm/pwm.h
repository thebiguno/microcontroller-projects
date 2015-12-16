/*
 * PWM motor control library, which allows you to control 4 motors.  Modified from
 * generic PWM library in Digital Cave microcontroller projects.
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

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * Initializes the PWM library using the specified ports / pins for motors.
 */
void pwm_init(volatile uint8_t* motorPort1, uint8_t motorPin1,
		volatile uint8_t* motorPort2, uint8_t motorPin2,
		volatile uint8_t* motorPort3, uint8_t motorPin3,
		volatile uint8_t* motorPort4, uint8_t motorPin4);
				
				
/*
 * Sets the overflow value for each of the motors.
 */
void pwm_set_values(uint8_t motorValue1, uint8_t motorValue2, uint8_t motorValue3, uint8_t motorValue4);

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

#if defined (__cplusplus)
}
#endif

#endif
