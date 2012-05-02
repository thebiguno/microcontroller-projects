#include "matrix.h"

using namespace digitalcave;

void Matrix::setFrame(uint8_t* frame){
	for(uint8_t i = 0; i < 64; i++){
		this->displayBuffer[i] = frame[i];
	}
}

uint8_t Matrix::getRedByte(uint8t_t row, uint8_t dutyCycle){
	uint8_t result = 0;
	for (uint8_t i = row * 8; i < (row + 1) * 8; i++){
		if (((this.displayBuffer[i]) >> 4) >= dutyCycle){
			result |= _BV(i);
		}
	}
	return result;
}

uint8_t Matrix::getGreenByte(uint8t_t row, uint8_t dutyCycle){
	uint8_t result = 0;
	for (uint8_t i = row * 8; i < (row + 1) * 8; i++){
		if (((this.displayBuffer[i]) & 0x0F) >= dutyCycle){
			result |= _BV(i);
		}
	}
	return result;
}
