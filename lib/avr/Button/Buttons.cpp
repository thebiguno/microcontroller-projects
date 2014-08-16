#include "Buttons.h"
#include <util/delay.h>

using namespace digitalcave;

Buttons::Buttons(volatile uint8_t *port, uint8_t pins) {
	this->port = port;
	this->pins_bv = pins;
	this->last = 0;
	
	*(port - 0x1) &= ~pins;	// input
	*(port - 0x2) |= pins;	// pull-up
}

uint8_t Buttons::poll() {
	volatile uint8_t *pin = this->port - 0x2;
	uint8_t press = 0x00;
	uint8_t release = 0xFF;
	for (uint8_t i = 0; i < 8; i++) {
		uint8_t x = *pin;
		press |= x;
		release &= x;
		_delay_ms(12);
	}
	this->current |= ~press;		// 0->1 for pressed
	this->current &= ~release;		// 1->0 for released
	this->current &= this->pins_bv;
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
