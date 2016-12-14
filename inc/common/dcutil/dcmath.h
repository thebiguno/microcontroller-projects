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
 * Lookup-table based acos implementation.
 * Returns an angle (in degrees) given the input value.  If the value is
 * outside of the domain [-1, 1] then it returns 255 to indicate NaN.
 */
float acos_f(float value);

/*
 * Lookup-table based cos implementation.
 * Returns the cosine value of the given angle (in degrees).
 */
float cos_f(float angle);

/*
 * Lookup-table based sin implementation.
 * Returns the sine value of the given angle (in degrees).
 */
float sin_f(float angle);

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
