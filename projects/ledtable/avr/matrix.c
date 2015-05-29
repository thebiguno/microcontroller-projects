#include "matrix.h"
#include <util/delay.h>

static ws2811_t buffer[144];

void set_pixel(int16_t x, int16_t y, pixel_t rgb, uint8_t overlay) {
	uint8_t i = (x & 0x01 == 0x00) ? (x * 36 + y * 3) : (x * 69 - y * 3);
	
	if (i >= 432 || i < 0) return;
	
	if (overlay == OVERLAY_REPLACE){
		buffer[i++] = rgb[1];
		buffer[i++] = rgb[0];
		buffer[i]   = rgb[2];
	}
	else if (overlay == OVERLAY_OR){
		buffer[i++] |= rgb[1];
		buffer[i++] |= rgb[0];
		buffer[i]   |= rgb[2];
	}
	else if (overlay == OVERLAY_NAND){
		buffer[i++] &= ~rgb[1];
		buffer[i++] &= ~rgb[0];
		buffer[i]   &= ~rgb[2];
	}
	else if (overlay == OVERLAY_XOR){
		buffer[i++] ^= rgb[1];
		buffer[i++] ^= rgb[0];
		buffer[i]   ^= rgb[2];
	}
}

void matrix_write_buffer(){
	ws2811_set(buffer, 144);
}

uint8_t get_pixel(int16_t x, int16_t y){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT || x < 0 || y < 0) return 0;	//Bounds check
	return buffer[x][y];
}