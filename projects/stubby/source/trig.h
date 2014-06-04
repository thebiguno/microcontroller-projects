#ifndef TRIG_H
#define TRIG_H

#include <avr/io.h>
/*
 * Returns an angle (in degrees) given the input value.  If the value is
 * outside of the domain [-1, 1] then it returns 255 to indicate NaN.
 */
uint8_t acos_d(double value);

/*
 * Returns the cosine value of the given angle (in degrees).
 */
double cos_d(int16_t angle);

/*
 * Returns the sine value of the given angle (in degrees).
 */
double sin_d(int16_t angle);

#endif