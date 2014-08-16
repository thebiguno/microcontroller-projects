#include "Buttons.h"
#include <util/delay.h>

using namespace digitalcave;

Buttons::Buttons(volatile uint8_t *port, uint8_t pins) {
	this->port = port;
	this->pins_bv = pins;
	this->lastState = 0;
	
	*(port - 0x1) &= ~pins;	// input
	*(port - 0x2) |= pins;	// pull-up
}

uint8_t Buttons::poll() {
	volatile uint8_t *pin = this->port - 0x2;
	this->state = 0x00;
	for (uint8_t i = 0; i < 8; i++) {
		this->state |= *pin;
		_delay_ms(12);
	}
	this->state = ~this->state & this->pins_bv;
	return this->state;
}

uint8_t Buttons::changed() {
	if (this->state != this->lastState) {
		uint8_t result = this->state ^ this->lastState;
		this->lastState = this->state;
		return result;
	} else {
		return 0;
	}
}

uint8_t Buttons::pressed() {
	return this->state;
}
