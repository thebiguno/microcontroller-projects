#ifndef MATRIX_H
#define MATRIX_H

struct pixel_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} pixel_t;

#include "../draw.h"

void matrix_write_buffer();

#endif