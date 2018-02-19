#include "NullStream.h"

using namespace digitalcave;

ConstStream::ConstStream(const uint8_t* data, const uint8_t len) :
	data(data),
	len(len),
	pos(0) {}

uint8_t ConstStream::read(uint8_t *c){
	return data[pos++];
}

uint8_t ConstStream::write(uint8_t b){
	return 0;
}

uint8_t ConstStream::reset() {
	pos = 0;
	return 1;
}
