#ifndef MATRIX_H
#define MATRIX_H

#include "lib/ws2811/ws2812.h"

#define MATRIX_WIDTH 12
#define MATRIX_HEIGHT 12

typedef ws2811_t pixel_t;

#include "lib/draw/draw.h"

void matrix_write_buffer();

#endif