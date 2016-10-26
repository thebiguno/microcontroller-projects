#ifndef DC_MATH
#define DC_MATH

#include <stdint.h>

#define fabs(x) __builtin_fabs(x)
#define degToRad(degrees) ((degrees) / 180.0 * M_PI)
#define radToDeg(radians) ((radians) * (180.0 / M_PI))

#if defined (__cplusplus)
extern "C" {
#endif

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

/*
 * Fast inverse square root function.
 */
float invSqrt(float x);

#if defined (__cplusplus)
}
#endif

#endif
