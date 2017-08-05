#include "FlashStream.h"
#include <avr/pgmspace.h>

using namespace digitalcave;

FlashStream::FlashStream(uint16_t position) {
	this->position = position;
}
FlashStream::~FlashStream() {
}

uint16_t FlashStream::skip(uint16_t n) {
	this->position += n;
	return n;
}

uint8_t FlashStream::read(uint8_t* b){
	*b = pgm_read_byte_near(position++);
	return 1;
}

// TODO for now just read-only
uint8_t FlashStream::write(uint8_t b) {
	return 0;
}
