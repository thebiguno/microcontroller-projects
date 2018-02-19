#include "Stream.h"

using namespace digitalcave;

uint8_t Stream::flush() {
	return 0;
}

uint8_t Stream::reset() {
	return 0;
}

uint16_t Stream::read(uint8_t* a, uint16_t len){
	uint16_t count = 0;
	uint8_t data = 0;

	while (count < len && read(&data)){
		a[count++] = data;
	}

	return count;
}

uint8_t Stream::write(const char* data){
	return write((char*) data);
}
uint8_t Stream::write(char *data){
	uint8_t i = 0;
	while (*data){
		if (!write(*data++)) return i;
		i++;
	}
	return i;
}

uint16_t Stream::write(uint8_t *data, uint16_t len){
	for (uint8_t i = 0; i < len; i++){
		if (!write(data[i])) return i;
	}
	return len;
}

uint16_t Stream::skip(uint16_t n) {
	uint16_t count = 0;
	uint8_t data = 0;

	while (count < (n - 1) && read(&data)){

	}

	return count;
}
