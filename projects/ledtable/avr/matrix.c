#include "matrix.h"

pixel_t buffer[144];

void set_pixel(int16_t x, int16_t y, pixel_t rgb, uint8_t overlay) {
	uint8_t i = ((x & 0x01) == 0x00) ? (x * 12 + y) : (x * 23 - y);
	
	if (i >= 144 || i < 0) return;
	
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

pixel_t get_pixel(uint8_t x, uint8_t y) {
	uint8_t i = ((x & 0x01) == 0x00) ? (x * 12 + y * 3) : (x * 23 - y * 3);
	
	if (i >= 144 || i < 0) return { .red = 0x00, .green = 0x00; .blue = 0x00 );

	return buffer[i];
}

void matrix_write_buffer(){
	ws281x_set(buffer);
}