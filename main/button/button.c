#include "button.h"

#include <avr/sfr_defs.h>

#define BUTTONS 3
#define SIZE 16

static volatile uint8_t *_hr_port = 0;
static volatile uint8_t *_mn_port = 0;
static volatile uint8_t *_mode_port = 0;

static uint8_t _hr_pin = 0;
static uint8_t _mn_pin = 0;
static uint8_t _mode_pin = 0;

static uint8_t _state = 0;
static uint8_t _changed = 0;

static uint16_t _registers[BUTTONS]; // SIZE corresponds to the number of bits in each register

void button_init(volatile uint8_t *hr_port, uint8_t hr_pin, volatile uint8_t *mn_port, uint8_t mn_pin, volatile uint8_t *mode_port, uint8_t mode_pin) {
	_hr_port = hr_port;
	_mn_port = mn_port;
	_mode_port = mode_port;

	// set ddr input
	*(_hr_port - 0x1) &= ~_BV(hr_pin);
	*(_mn_port - 0x1) &= ~_BV(mn_pin);
	*(_mode_port - 0x1) &= ~_BV(mode_pin);
	
	// enable pull-up resistors
	*_hr_port |= _BV(hr_pin);
	*_mn_port |= _BV(mn_pin);
	*_mode_port |= _BV(mode_pin);
	
	_hr_pin = hr_pin;
	_mn_pin = mn_pin;
	_mode_pin = mode_pin;
	
	// initialize registers to 0xFF (unpressed for last 8 samples)
	for (uint8_t i = 0; i < SIZE; i++) {
		_registers[i] = 0xFF;
	}
}

// this method should be called every millisecond
void button_sample() {
	// shift registers towards the most significant bit
	for (uint8_t i = 0; i < BUTTONS; i++) {
		_registers[i] <<= 1;
	}
	
	// set the least significant bit to the current switch state (0 == pressed)
	if (*(_hr_port - 0x2) & _BV(_hr_pin)) {
		_registers[BUTTON_HOUR] |= 1;
	}
	if (*(_mn_port - 0x2) & _BV(_mn_pin)) {
		_registers[BUTTON_MIN] |= 1;
	}
	if (*(_mode_port - 0x2) & _BV(_mode_pin)) {
		_registers[BUTTON_MODE] |= 1;
	}
	
	uint8_t debounced = 0;
	for (uint8_t i = 0; i < BUTTONS; i++) {
		// if register value is 0 then set debounced to pressed (0 == consistently pressed for last 16 reads)
		if (_registers[i] == 0) debounced |= _BV(i);
	}
	_changed = debounced ^ _state;
	_state = debounced;
}

uint8_t button_state() {
	return _state;
}

uint8_t button_changed() {
	return _changed;
}