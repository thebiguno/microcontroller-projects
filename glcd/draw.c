#include "draw.h"

// Implementation of Bresenham's algorithm; adapted from Lady Ada's GLCD library,
// which was in turn adapted from Wikpedia.
void glcd_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t o){

	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}

	for (; x0 < x1; x0++) {
		if (steep) {
			glcd_set_pixel(y0, x0, o);
		}
		else {
			glcd_set_pixel(x0, y0, o);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

void glcd_draw_square(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t f, uint8_t o){
	//Make sure that x0,y0 is top left corner.
	if (x0 > x1) swap(x0, x1);
	if (y0 > y1) swap(y0, y1);

	for(uint8_t x = x0; x <= x1; x++){
		for (uint8_t y = y0; y <= y1; y++){
			if (f || x == x0 || x == x1 || y == y0 || y == y1) glcd_set_pixel(x, y, o);
		}
	}
}