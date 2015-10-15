#include "Matrix.h"

using namespace digitalcave;

Matrix::Matrix() {
}

Matrix::~Matrix() {
}

void Matrix::setColor(Rgb rgb) {
	color.red = rgb.getRed();
	color.green = rgb.getGreen();
	color.blue = rgb.getBlue();
}

void Matrix::setColor(uint8_t r, uint8_t g, uint8_t b) {
	color.red = r;
	color.green = g;
	color.blue = b;
}

void Matrix::setPixel(int16_t x, int16_t y) {
	int16_t i = (x & 0x01) ? (x * MATRIX_WIDTH + MATRIX_HEIGHT - 1 - y) : (x * MATRIX_HEIGHT + y);
	//int16_t i = (x * 12) + y;
	if (i >= MATRIX_WIDTH * MATRIX_HEIGHT || i < 0) return;
	
	ws2812_t current;
	current.red = buffer[i].red;
	current.green = buffer[i].green;
	current.blue = buffer[i].blue;

	uint8_t overlay = getOverlay();
	if (overlay == DRAW_OVERLAY_REPLACE){
		buffer[i].red   = color.red;
		buffer[i].green = color.green;
		buffer[i].blue  = color.blue;
	}
	else if (overlay == DRAW_OVERLAY_OR){
		buffer[i].red   |= color.red;
		buffer[i].green |= color.green;
		buffer[i].blue  |= color.blue;
	}
	else if (overlay == DRAW_OVERLAY_NAND){
		buffer[i].red   &= ~color.red;
		buffer[i].green &= ~color.green;
		buffer[i].blue  &= ~color.blue;
	}
	else if (overlay == DRAW_OVERLAY_XOR){
		buffer[i].red   ^= color.red;
		buffer[i].green ^= color.green;
		buffer[i].blue  ^= color.blue;
	}
	
	if (buffer[i].red != current.red || buffer[i].green != current.green || buffer[i].blue != current.blue) changed = 1;
	
//	buffer[x*12+y].red = 0xff;
}

void Matrix::flush(){
	if (changed) {
		ws281x_set(buffer);
		changed = 0;
	}
}