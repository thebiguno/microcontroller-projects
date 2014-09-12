#ifndef TRIG_H
#define TRIG_H

#ifndef DEBUG_SIMULATION
#include <avr/io.h>
#else
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#endif
#include <math.h>

/*
 * Returns an angle (in degrees) given the input value.  If the value is
 * outside of the domain [-1, 1] then it returns 255 to indicate NaN.
 */
double acos_f(double value);

/*
 * Returns the cosine value of the given angle (in degrees).
 */
double cos_f(double angle);

/*
 * Returns the sine value of the given angle (in degrees).
 */
double sin_f(double angle);

/*
 * Fast square root function; from http://www.mikrocontroller.net/articles/AVR_Arithmetik#avr-gcc_Implementierung_.2816_Bit.29
 */
uint16_t sqrt_f(uint16_t q);

#endif
