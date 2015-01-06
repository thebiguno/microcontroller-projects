#include "Buttons.h"
#include <util/delay.h>

using namespace digitalcave;

Buttons::Buttons(volatile uint8_t *port, uint8_t pins, uint8_t pressed, uint8_t released) {
	this->port = port;
	this->pin = this->port - 0x2;
	this->pins_bv = pins;
	this->pressed_bv = 0xff;
	for (uint8_t i = 0; i < pressed; i++) {
		pressed_bv <<= 1;
	}
	this->pressed_bv = ~this->pressed_bv;
	this->released_bv = 0xff;
	for (uint8_t i = 8; i < released; i++) {
		released_bv <<= 1;
	}
	this->released_bv = ~this->released_bv;
	this->last = 0x00;
	this->current = 0x00;
	for (uint8_t i = 0; i < 8; i++) {
		this->window[i] = 0xff;
	}
	*(port - 0x1) &= ~pins;	// input
	*this->pin |= pins;		// pull-up
}

uint8_t Buttons::sample() {
	uint8_t x = *this->pin;
	for (uint8_t i = 0; i < 8; i++) {
		x >>= 1;
		if (this->pins_bv & _BV(i)) {										// only check the pins that are of interest
			uint8_t v = (this->window[i] << 1) | (x & _BV(1));				// shift and put latest reading in least significant bit
			this->window[i] = v;
			if ((v & this->pressed_bv) == 0x00) {							// check for change from unpressed (1) to pressed (0)
				this->current |= _BV(i);									// mark button as pressed
			} else if ((v & this->released_bv) == this->released_bv) {		// check for change from pressed (0) to unpressed (1)
				this->current &= ~_BV(i);									// mark button released
			}
		}
	}
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
