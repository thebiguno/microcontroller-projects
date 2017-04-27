#ifndef ws281x_h
#define ws281x_h

#include <stdint.h>

typedef struct ws2811_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} ws2811_t;

void ws2811_set(const void *values);
	
#endif
