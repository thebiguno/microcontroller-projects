#include "button.h"

#include <avr/sfr_defs.h>

#define BUTTONS 2
#define SIZE 8

static volatile uint8_t *_play_pause_port = 0;
static volatile uint8_t *_slide_count_port = 0;

static uint8_t _play_pause_pin = 0;
static uint8_t _slide_count_pin = 0;

static uint8_t _state = 0;
static uint8_t _changed = 0;

static uint8_t _registers[BUTTONS];

void button_init(volatile uint8_t *play_pause_port, uint8_t play_pause_pin, volatile uint8_t *slide_count_port, uint8_t slide_count_pin) {
	_play_pause_port = play_pause_port;
	_slide_count_port = slide_count_port;

	// set ddr input
	*(_play_pause_port - 0x1) &= ~_BV(play_pause_pin);
	*(_slide_count_port - 0x1) &= ~_BV(slide_count_pin);
	
	// enable pull-up resistors
	*_play_pause_port |= _BV(play_pause_pin);
	*_slide_count_port |= _BV(slide_count_pin);
	
	_play_pause_pin = play_pause_pin;
	_slide_count_pin = slide_count_pin;
	
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
	if (*(_play_pause_port - 0x2) & _BV(_play_pause_pin)) {
		_registers[BUTTON_PLAY_PAUSE] |= 1;
	}
	if (*(_slide_count_port - 0x2) & _BV(_slide_count_pin)) {
		_registers[BUTTON_SLIDE_COUNT] |= 1;
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