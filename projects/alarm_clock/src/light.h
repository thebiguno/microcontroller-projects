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
 * Sets the brightness for the lights.  Brightness ranges from 0 to 100.
 */
void light_set(uint8_t brightness);

/*
 * Gets the last brightness set in light_set, or 0 if light_state is 0.
 */
uint8_t light_get();

/*
 * Turns on the timer clock.
 */
void light_on();

/*
 * Turns off the light pins and stops the timer clock.
 */
void light_off();

/*
 * Toggles the light on or off, depending on last state
 */
void light_toggle();

/*
 * Returns 0 if light is off, 1 if light is on
 */
uint8_t light_state();

#if defined (__cplusplus)
}
#endif

#endif
