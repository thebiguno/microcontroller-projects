#include "button.h"

#include <avr/sfr_defs.h>

static uint8_t _state = 0;
static uint8_t _changed = 0;

static volatile uint8_t *_hr_port = 0;
static volatile uint8_t *_mn_port = 0;
static volatile uint8_t *_mode_port = 0;

static uint8_t _hr_pin = 0;
static uint8_t _mn_pin = 0;
static uint8_t _mode_pin = 0;

static uint8_t _debounced = 0;
static uint32_t _ms = 0;
static uint8_t _index = 0;
static uint8_t _samples[4];

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
}

uint8_t button_read(uint32_t ms) {
	// sample every 10 ms, debounced state is equal to the last 4 samples (i.e. must be consistent for 40 ms)
	
	if (_ms - ms > 10) {
		_ms = ms;
		_samples[_index] = 0;
		if (*_hr_port & _BV(_hr_pin)) {
			_samples[_index] |= BUTTON_HOUR;
		}
		if (*_mn_port & _BV(_mn_pin)) {
			_samples[_index] |= BUTTON_MIN;
		}
		if (*_mode_port & _BV(_mode_pin)) {
			_samples[_index] |= BUTTON_MODE;
		}
		_index++;
		if (_index > 4) _index = 0;
		_debounced = 0xff;
		for (uint8_t i = 0; i < 4; i++) {
			_debounced &= _samples[_index];
		}
	}
	return _debounced;
}