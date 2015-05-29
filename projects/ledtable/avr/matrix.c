#include "matrix.h"

pixel_t buffer[144];

void set_pixel(int16_t x, int16_t y, pixel_t rgb, uint8_t overlay) {
	uint8_t i = ((x & 0x01) == 0x00) ? (x * 12 + y * 3) : (x * 23 - y * 3);
	
	if (i >= 432 || i < 0) return;
	
	if (overlay == OVERLAY_REPLACE){
		buffer[i].red   = rgb.red;
		buffer[i].green = rgb.green;
		buffer[i].blue  = rgb.blue;
	}
	else if (overlay == OVERLAY_OR){
		buffer[i].red   |= rgb.red;
		buffer[i].green |= rgb.green;
		buffer[i].blue  |= rgb.blue;
	}
	else if (overlay == OVERLAY_NAND){
		buffer[i].red   &= ~rgb.red;
		buffer[i].green &= ~rgb.green;
		buffer[i].blue  &= ~rgb.blue;
	}
	else if (overlay == OVERLAY_XOR){
		buffer[i].red   ^= rgb.red;
		buffer[i].green ^= rgb.green;
		buffer[i].blue  ^= rgb.blue;
	}
}

void matrix_write_buffer(){
	ws281x_set(buffer);
}