#include "matrix.h"

uint8_t changed;
pixel_t buffer[144];

void set_pixel(int16_t x, int16_t y, pixel_t rgb, uint8_t overlay) {
	uint8_t i = ((x & 0x01) == 0x00) ? (x * 12 + y) : (x * 23 - y);
	
	if (i >= 144 || i < 0) return;
	
	pixel_t current;
	current.red = buffer[i].red;
	current.green = buffer[i].green;
	current.blue = buffer[i].blue;
	
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
	
	if (buffer[i].red != current.red || buffer[i].green != current.green || buffer[i].blue != current.blue) changed = 1;
}

pixel_t get_pixel(uint8_t x, uint8_t y) {
	uint8_t i = ((x & 0x01) == 0x00) ? (x * 12 + y * 3) : (x * 23 - y * 3);
	
	if (i >= 144 || i < 0) {
		pixel_t black;
		return black;
	}

	return buffer[i];
}

void matrix_write_buffer(){
	if (changed) {
		ws281x_set(buffer);
		changed = 0;
	}
}