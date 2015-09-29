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

void Stream::write(const char* data){
	write((char*) data);
}
void Stream::write(char *data){
	while (*data){
		write(*data++);
	}
}

void Stream::write(uint8_t *data, uint8_t len){
	for (uint8_t i = 0; i < len; i++){
		write(data[i]);
	}
}
