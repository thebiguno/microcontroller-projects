#include "button.h"
using namespace std;

Button::Button(volatile uint8_t **ports, uint8_t *pins, uint8_t size = 1, uint8_t count = 8){
	this->size = size;
	this->count = count;
	this->counters = (uint8_t*) malloc(size);
	this->ports = ports;
	this->pins = pins;
}

void Button::poll(){
	for (uint8_t i = 0; i < this->size; i++){
		if ((*this->ports[i] | this->pins[i]) == 0){
			this->counters[i]++;
			//Prevent counter overflow
			if (this->counters[i] > this->count) this->counters[i] = this->count;
		}
		else {
			this->counters[i] = 0;
		}
	}
}

uint8_t Button::isPressed(uint8_t index){
	if (index > this->size) return 0;
	
	return (this->counters[index] > this->count);
}