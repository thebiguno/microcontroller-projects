#ifndef ws281x_h
#define ws281x_h

#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif
	
typedef struct ws2812_t {
	uint8_t green;
	uint8_t red;
	uint8_t blue;
} ws2812_t;

void ws281x_set(const void *values);

#if defined (__cplusplus)
}
#endif
	
#endif
