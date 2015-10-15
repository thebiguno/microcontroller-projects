#include "Matrix.h"

using namespace digitalcave;

static uint8_t twi_master_tx_writer(uint16_t i){
	if (i == 0){
		return depth;
	}
	else {
		uint8_t result = 0x00;
		if (depth == 0) {
			// return the byte as is
			// ggggrrrr
			result = ((uint8_t*) buffer)[i-1];
		} else if (depth == 1) {
			uint16_t idx = (i - 1) * 2;
			//xxxxggrr xxxxggrr -> ggrrggrr
			//combine two bytes in the buffer into on byte in the message
			result = (((uint8_t*) buffer)[idx] << 4) & 0xC0; // bits 6,7
			result |= (((uint8_t*) buffer)[idx] << 4) & 0x30; // bits 4,5
			idx++;
			result |= ((uint8_t*) buffer)[idx] & 0x0C; // bits 2,3
			result |= ((uint8_t*) buffer)[idx] & 0x03; // bits 0,1
			// result = 0xc4;
		} else if (depth == 2) {
			// TODO
//			result = 0x33;
		}
		return result;
	}
}

Matrix::Matrix() {
	twi_init();
	twi_attach_master_tx_writer(twi_master_tx_writer);
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