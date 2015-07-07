#ifndef MATRIX_H
#define MATRIX_H

#include "lib/draw/draw.h"
#include "lib/ws281x/ws2812.h"

#define MATRIX_WIDTH 12
#define MATRIX_HEIGHT 12

void draw_set_value(ws2812_t value);
void draw_flush();

#endif