#ifndef DC_MATH
#define DC_MATH

#include <stdint.h>

#define degToRad(degrees) ((degrees) / 180.0 * M_PI)
#define radToDeg(radians) ((radians) * (180.0 / M_PI))

#if defined (__cplusplus)
extern "C" {
#endif

float invSqrt(float x);

#if defined (__cplusplus)
}
#endif

#endif