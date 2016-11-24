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
void motor_set(double motorValue1, double motorValue2, double motorValue3, double motorValue4, double motorValue5, double motorValue6, double motorValue7, double motorValue8);

/*
 * Turns on the PWM.
 */
void motor_start();

/*
 * Turns off the PWM and takes all motors to off state.
 */
void motor_stop();

#if defined (__cplusplus)
}
#endif

#endif
