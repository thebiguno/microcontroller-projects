#include "Buttons.h"
#include <util/delay.h>

using namespace digitalcave;

Buttons::Buttons(volatile uint8_t *port, uint8_t pins, uint8_t pressed, uint8_t released) {
	this->port = port;
	this->pin = this->port - 0x2;
	this->pins_bv = pins;
	this->last = 0x00;
	this->current = 0x00;
	for (uint8_t i = 0; i < 8; i++) {
		this->window[i] = 0xff;
	}
	*(port - 0x1) &= ~this->pins_bv;		// DDR set to input
	*this->port |= this->pins_bv;		// Set pull-ups
}

uint8_t Buttons::sample() {
	uint8_t sampled_pins = ~*this->pin;		//We invert the pins since it is active low
	
	for (uint8_t i = 0; i < 8; i++) {
		if (this->pins_bv & _BV(i)) {										// only check the pins that are of interest
			this->window[i] <<= 1;											// Shift the window one left
			uint8_t sampled_pin_high = sampled_pins & _BV(i);				// See if this particular pin is high
			if (sampled_pin_high) this->window[i] |= 0x01;					// If so, add it to the LSB of the window
			if (this->window[i] == 0xFF){									// If it has been presesd for 8 cycles...
				this->current |= _BV(i);									// mark button as pressed
			}
			else if (this->window[i] == 0x00){								// If it has been released for 8 cycles...
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
