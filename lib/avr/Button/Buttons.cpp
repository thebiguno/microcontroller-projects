#include "Buttons.h"
#include <util/delay.h>

using namespace digitalcave;

Buttons::Buttons(volatile uint8_t *port, uint8_t pin_mask, uint8_t press_time, uint8_t release_time, uint8_t hold_time, uint8_t repeat_time){
	this->port = port;
	this->pin = this->port - 0x2;
	this->pins_bv = pins;
	this->pressed_bv = 0xff;
	for (uint8_t i = 0; i < press_time; i++) {
		pressed_bv >>= 1;
	}
	this->pressed_bv = ~this->pressed_bv;
	this->released_bv = 0xff;
	for (uint8_t i = 8; i < release_time; i++) {
		released_bv >>= 1;
	}
	this->released_bv = ~this->released_bv;
	
	this->hold_time = hold_time;
	this->repeat_time = repeat_time;
	//0xFF is reserved state.
	if (this->hold_time == 0xFF) this->hold_time = 0xFE;
	if (this->repeat_time == 0xFF) this->repeat_time = 0xFE;
	
	this->last = 0x00;
	this->current = 0x00;
	for (uint8_t i = 0; i < 8; i++) {
		this->window[i] = 0x00;
	}
	*(port - 0x1) &= ~this->pins_bv;		// DDR set to input
	*this->port |= this->pins_bv;		// Set pull-ups
}

Buttons::Buttons(volatile uint8_t *port, uint8_t pin_mask) : Buttons::Buttons(port, pin_mask, 8, 8, 0, 0) {}

uint8_t Buttons::sample() {
	uint8_t sampled_pins = ~*this->pin;		//We invert the pins since it is active low
	
	this->hold = 0x00;
	this->repeat = 0x00;
	
	for (uint8_t i = 0; i < 8; i++) {
		if (this->pins_bv & _BV(i)) {										// only check the pins that are of interest
			this->window[i] <<= 1;											// Shift the window one left
			uint8_t sampled_pin_high = sampled_pins & _BV(i);				// See if this particular pin is high
			if (sampled_pin_high) this->window[i] |= 0x01;					// If so, add it to the LSB of the window
			if ((this->window[i] & this->pressed_bv) == this->pressed_bv){	// If it has been pressed for <pressed> cycles...
				this->current |= _BV(i);									// mark button as pressed
			}
			else if ((this->window[i] & this->released_bv) == 0x00){		// If it has been released for <released> cycles...
				this->current &= ~_BV(i);									// mark button released
			}
			//We only increment held timer if the button is currently pressed.
			if (this->current & _BV(i)){
				if (this->hold_timer[i] == 0xFF){
					//Do nothing here; we are already in 'held' state.
				}
				else if (this->hold_timer[i] >= this->hold_time){
					//If the button has been held for the required time, mark it as fully held (0xFF)
					this->hold_timer[i] = 0xFF;
					this->hold |= _BV(i);
				}
				else {
					//Increment timer
					this->hold_timer[i]++;
				}
			}
			else {
				//If the button is not currently pressed, then reset the timer
				this->hold_timer[i] = 0x00;
			}
			
			//We only lok at repeat options if the button is currently held (hold_timer == 0xFF)
			if (this->hold_timer[i] == 0xFF){
				this->repeat_timer[i]++;
				
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
