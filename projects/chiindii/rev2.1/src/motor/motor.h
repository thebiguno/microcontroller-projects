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

#include <math.h>
#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*
 * Sets the phase for each of the motors.  Limited to 10 bit values 1023 is 100%, 512 is 50%, etc.
 */
void motor_set(uint16_t motorValue1, uint16_t motorValue2, uint16_t motorValue3, uint16_t motorValue4);

/*
 * Turns on the timer clock.
 */
void motor_start();

/*
 * Turns off the motor pins and stops the timer clock.
 */
void motor_stop();

#if defined (__cplusplus)
}
#endif

#endif
