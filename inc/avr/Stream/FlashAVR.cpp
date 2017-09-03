#include "FlashAVR.h"
#include <avr/pgmspace.h>

using namespace digitalcave;

FlashAVR::FlashAVR(uint16_t position) {
	this->position = position;
	this->origin = position;
}
FlashAVR::~FlashAVR() {
}

void FlashAVR::reset() {
	this->position = this->origin;
}

uint16_t FlashAVR::skip(uint16_t n) {
	this->position += n;
	return n;
}

uint8_t FlashAVR::read(uint8_t* b){
	*b = pgm_read_byte_near(position++);
	return 1;
}

// TODO for now just read-only
uint8_t FlashAVR::write(uint8_t b) {
	return 0;
}
