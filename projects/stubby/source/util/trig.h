#ifndef TRIG_H
#define TRIG_H

#include <avr/io.h>
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

#endif
