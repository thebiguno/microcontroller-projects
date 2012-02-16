#include "button.h"

#include <avr/sfr_defs.h>

static volatile uint8_t *_hr_port = 0;
static volatile uint8_t *_mn_port = 0;
static volatile uint8_t *_mode_port = 0;

static uint8_t _hr_pin = 0;
static uint8_t _mn_pin = 0;
static uint8_t _mode_pin = 0;

static uint8_t _state = 0;
static uint8_t _changed = 0;

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

void button_read(uint32_t ms) {
	// sample every 5 ms, debounced state is equal to the last 4 samples (i.e. must be consistent for 40 ms)
	
	static uint32_t _ms;
	static uint8_t _index;
	static uint8_t _samples[8];
	static uint8_t flag;
	
	if (ms - _ms > 5) {
		if (flag == 0) {
			flag = 1;

			_ms = ms;
			_samples[_index] = 0;
			// if (!(*(_hr_port - 0x2) & _BV(_hr_pin))) {
			// 			_samples[_index] |= BUTTON_HOUR;
			// 		}
			// 		if (!(*(_mn_port - 0x2) & _BV(_mn_pin))) {
			// 			_samples[_index] |= BUTTON_MIN;
			// 		}
			if (!(*(_mode_port - 0x2) & _BV(_mode_pin))) {
				_samples[_index] |= BUTTON_MODE;
			}
			_index++;
			if (_index > 8) _index = 0;
			uint8_t debounced = 0xff;
			for (uint8_t i = 0; i < 8; i++) {
				debounced &= _samples[i];
			}
			_changed = _state ^ debounced;
			_state = debounced;
		}
	} else {
		flag = 0;
	}
}

uint8_t button_state() {
	return _state;
}

uint8_t button_changed() {
	return _changed;
}