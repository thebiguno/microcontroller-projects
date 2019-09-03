/*
 * PWM light control library, which allows you to control 3 separate lights, changing brightness and
 * white balance.  Modified from generic PWM library in Digital Cave microcontroller projects.
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

#define LIGHT_PORT			PORTD
#define LIGHT_Y_PIN			PORTD5
#define LIGHT_N_PIN			PORTD6
#define LIGHT_B_PIN			PORTD7

/*
 * Initialize the light pins
 */
void light_init();

/*
 * Sets the brightness and white balance for the lights.  Brightness ranges from 0 to 1.  White balance
 * ranges from -1 (yellow) to 0 (neutral) to 1 (blue).
 */
void light_set(double brightness, double whiteBalance);

/*
 * Turns on the timer clock.
 */
void light_on();

/*
 * Turns off the light pins and stops the timer clock.
 */
void light_off();

#if defined (__cplusplus)
}
#endif

#endif
