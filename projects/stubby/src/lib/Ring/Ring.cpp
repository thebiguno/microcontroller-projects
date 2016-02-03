#include "Ring.h"

Ring::Ring(uint8_t size) {
	this->buffer = (uint8_t*) malloc(size);
	this->size = size;
	this->head = 0x00;
	this->tail = 0x00;
}

uint8_t Ring::isEmpty(){
	return (this->head == this->tail);
}

uint8_t Ring::isFull(){
	return ((this->head + 1) % this->size == this->tail);
}

uint8_t Ring::get(){
	char c = this->buffer[this->tail];
	if (++this->tail >= this->size) this->tail = 0;
	return c;
}

void Ring::put(uint8_t data){
	this->buffer[this->head] = data;
	if (++this->head >= this->size) this->head = 0;
}