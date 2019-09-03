/*
 * PWM light control library, which allows you to control up to 4 separate lights.  Modified from
 * generic PWM library in Digital Cave microcontroller projects.
 *
 * Written by Wyatt Olson and Warren Janssens
 * http://digitalcave.ca
 * Released under the Creative Commons Attribution-NonCommercial-ShareAlike license.
 */

#ifndef LIGHT_H
#define LIGHT_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdlib.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define LIGHT_PORT			PORTB
#define LIGHT_Y_PIN			PORTB4
#define LIGHT_N_PIN			PORTB5
#define LIGHT_B_PIN			PORTB6

/*
 * Sets the brightness and white balance for the lights.  Brightness ranges from 0 to 1.  White balance
 * ranges from -1 (yellow) to 0 (neutral) to 1 (blue).
 */
void light_set(double brightness, double whiteBalance);

/*
 * Turns on the timer clock.
 */
void light_start();

/*
 * Turns off the light pins and stops the timer clock.
 */
void light_stop();

#if defined (__cplusplus)
}
#endif

#endif
