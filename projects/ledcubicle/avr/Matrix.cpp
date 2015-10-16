#include "Matrix.h"

using namespace digitalcave;

Matrix::Matrix() {
	twi_init();
}

Matrix::~Matrix() {
}

void Matrix::setColor(uint8_t gr) {
	this->color = gr;
}
void Matrix::setColor(uint8_t red, uint8_t green) {
	red /= 16;
	green /= 16;
	this->color = (green << 4) | red;
}

void Matrix::setPixel(int16_t x, int16_t y) {
	this->changed = 1;
	
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT || x < 0 || y < 0) return;  //Bounds check

	uint8_t index = x * MATRIX_WIDTH + y + 1;
	if (overlay == DRAW_OVERLAY_REPLACE){
		buffer[index] = color;
	}
	else if (overlay == DRAW_OVERLAY_OR){
		buffer[index] |= color;
	}
	else if (overlay == DRAW_OVERLAY_NAND){
		buffer[index] &= ~color;
	}
	else if (overlay == DRAW_OVERLAY_XOR){
		buffer[index] ^= color;
	}
}

void Matrix::flush() {
	if (changed) {
		twi_write_to(MATRIX_DRIVER_SLAVE_ADDRESS, buffer, MATRIX_BUFFER_LENGTH, TWI_BLOCK, TWI_STOP);
		this->changed = 0;
	}
}