#include "Stream.h"

using namespace digitalcave;

uint8_t Stream::read(uint8_t* a, uint8_t len){
	uint8_t count = 0;
	uint8_t data = 0;
	
	while (count < (len - 1) && read(&data)){
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

uint8_t Stream::write(uint8_t *data, uint8_t len){
	for (uint8_t i = 0; i < len; i++){
		if (!write(data[i])) return i;
	}
	return len;
}
