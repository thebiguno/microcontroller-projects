#include "ArrayStream.h"

using namespace digitalcave;

ArrayStream::ArrayStream(uint8_t size) {
	data = (uint8_t*) malloc(size);
	this->size = size;
	head = 0x00;
	tail = 0x00;
}

uint8_t ArrayStream::isEmpty(){
	return (head == tail);
}

uint8_t ArrayStream::isFull(){
	return ((head + 1) % size == tail);
}

uint8_t ArrayStream::read(uint8_t* b){
	if (head == tail) return 0;
	*b = data[tail];
	if (++tail >= size) tail = 0;
	return 1;
}

uint8_t ArrayStream::write(uint8_t b){
	if (((head + 1) % size == tail)) return 0;
	data[head] = b;
	if (++head >= size) head = 0;
	return 1;
}