#include "Buttons.h"
#include <util/delay.h>

using namespace digitalcave;

Buttons::Buttons(volatile uint8_t *port, uint8_t pins) {
	this->port = port;
	this->pin = this->port - 0x2;
	this->pins_bv = pins;
	this->last = 0x00;
	this->press = 0x00;
	this->release = 0xFF;
	
	*(port - 0x1) &= ~pins;	// input
	*this->pin |= pins;		// pull-up
}

uint8_t Buttons::poll() {
	for (uint8_t i = 0; i < 10; i++) {
		this->sample();
		_delay_ms(10);
	}
	return this->integrate();
}

void Buttons::sample() {
	uint8_t x = *this->pin;
	this->press |= x;
	this->release &= x;
}
uint8_t Buttons::integrate() {
	this->current |= ~press;		// 0->1 for pressed
	this->current &= ~release;		// 1->0 for released
	this->current &= this->pins_bv;
	this->press = 0x00;
	this->release = 0xFF;
	return this->current;
}

uint8_t Buttons::integratePressed() {
	this->current |= ~press;		// 0->1 for pressed
	this->current &= this->pins_bv;
	this->press = 0x00;
	return this->current;
}

uint8_t Buttons::integrateReleased() {
	this->current &= ~release;		// 1->0 for released
	this->current &= this->pins_bv;
	this->release = 0xFF;
	return this->current;
}

uint8_t Buttons::changed() {
	if (this->current != this->last) {
		uint8_t result = this->current ^ this->last;
		this->last = this->current;
		return result;
	} else {
		return 0;
	}
}

uint8_t Buttons::pressed() {
	return this->current;
}
