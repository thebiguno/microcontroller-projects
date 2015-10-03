#include "ArrayStream.h"

using namespace digitalcave;

ArrayStream::ArrayStream(uint8_t capacity) {
	if (capacity == 0) capacity = 1;	//Only YOU can prevent divide by zero exceptions!
	data = (uint8_t*) malloc(capacity);
	this->capacity = capacity;
	head = 0x00;
	tail = 0x00;
}
ArrayStream::~ArrayStream() {
	free((void*) data);
}

uint8_t ArrayStream::remaining() {
	return capacity - size();
}

uint8_t ArrayStream::size() {
	return (head > tail) ? head - tail : capacity - tail + head;
}

uint8_t ArrayStream::isEmpty(){
	return (head == tail);
}

uint8_t ArrayStream::isFull(){
	return ((head + 1) % capacity == tail);
}

uint8_t ArrayStream::read(uint8_t* b){
	if (isEmpty()) return 0;
	*b = data[tail];
	if (++tail >= capacity) tail = 0;
	return 1;
}

uint8_t ArrayStream::write(uint8_t b){
	if (isFull()) return 0;
	data[head] = b;
	if (++head >= capacity) head = 0;
	return 1;
}