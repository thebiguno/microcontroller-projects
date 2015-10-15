#include "Matrix.h"

using namespace digitalcave;

Matrix::Matrix() {
	twi_init();
}

Matrix::~Matrix() {
}

void Matrix::setDepth(uint8_t d) {
	this->depth = d;
}
void Matrix::setColor(uint8_t gr) {
	this->color = gr;
}
void Matrix::setColor(uint8_t red, uint8_t green) {
	if (this->depth == 0) {
		red /= 16;
		green /= 16;
		this->color = (green << 4) | red;
	}
	else if (this->depth == 1) {
		red /= 64;
		green /= 64;
		this->color = (green << 2) | red;
	}
	else if (this->depth == 2) {
		red /= 128;
		green /= 128;
		this->color = (green << 1) | red;
	}
}

void Matrix::setPixel(int16_t x, int16_t y) {
	this->changed = 1;
	
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT || x < 0 || y < 0) return;  //Bounds check

	if (overlay == DRAW_OVERLAY_REPLACE){
		buffer[x][y] = color;
	}
	else if (overlay == DRAW_OVERLAY_OR){
		buffer[x][y] |= color;
	}
	else if (overlay == DRAW_OVERLAY_NAND){
		buffer[x][y] &= ~color;
	}
	else if (overlay == DRAW_OVERLAY_XOR){
		buffer[x][y] ^= color;
	}
}

void Matrix::flush() {
	if (changed) {
		uint16_t l = MATRIX_LENGTH;
		if (depth == 0x01) l = l >> 1;
		else if (depth == 0x02) l = l >> 2;
		l++;
		twi_write_to(MATRIX_DRIVER_SLAVE_ADDRESS, (uint8_t*) buffer, l, TWI_BLOCK, TWI_STOP);
		this->changed = 0;
	}
}