#include "button.h"

using namespace digitalcave;

Button::Button(volatile uint8_t *port, uint8_t idx, uint8_t debounceCount){
	this->debounceCount = debounceCount;
	this->counter = 0;
	this->port = port;
	this->pin = port - 0x2;
	this->idx = _BV(idx);
	this->lastState = 0;
	
	*port |= _BV(idx);
}

void Button::poll(){
	if ((*this->pin & this->idx) == 0){
		this->counter++;
		//Prevent counter overflow
		if (this->counter > this->debounceCount) this->counter = this->debounceCount;
	}
	else {
		this->counter = 0;
	}
}

uint8_t Button::isChanged(){
	uint8_t state = this->isPressed();
	if (state != this->lastState){
		this->lastState = state;
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t Button::isPressed(){
	return (this->counter >= this->debounceCount);
}