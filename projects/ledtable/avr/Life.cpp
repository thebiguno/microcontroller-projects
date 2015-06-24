#include "Life.h"
#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/timer/timer.h"
#include <util/delay.h>
#include <stdlib.h>

using namespace digitalcave;

Life::Life() {
}

Life::~Life() {
}

void Life::run() {
	uint16_t buttons; 
	uint8_t running = 1;
	while (running) {
		for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
			for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
				uint8_t count = getNeighborCount(x, y);
				if (count == 3) {
					// birth
					setState(x, y, 0x01);
				}
				else if (count == 2 || count == 3) {
					// staying alive
					setState(x, y, 0x01);
				}
				else {
					setState(x, y, 0x00);
				}
			}
		}

		flush();
		matrix_write_buffer();

		//Store board hash
		for (uint8_t i = LIFE_HASH_COUNT - 1; i > 0; i--) {
			hashes[i] = hashes[i - 1];
		}
		hashes[0] = getStateHash();

		uint8_t matches = 0;
		for (uint8_t i = 0; i < LIFE_HASH_COUNT; i++) {
			for (uint8_t j = i + 1; j < LIFE_HASH_COUNT; j++) {
				if (hashes[i] == hashes[j]) matches++;
			}
		}
		if (matches == 0) matches = 0;
		else matches++;

		if (matches >= LIFE_MATCH_COUNT) {
			reset();
		}
		
		void psx_read_gamepad();
		buttons = psx_buttons();
		if (buttons & PSB_TRIANGLE) {
			running = 0;
		}

		_delay_ms(70);
		this->hue++;
		this->hue &= 360;
	}
}

void Life::setState(uint8_t x, uint8_t y, uint8_t value) {
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;	//Bounds check
	state[x][y] = value;
}

uint8_t Life::getState(uint8_t x, uint8_t y) {
	if (x < 0 || y < 0 || x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0x00;
	return state[x][y]; 
}

uint8_t Life::getNeighborCount(uint8_t x, uint8_t y) {
	uint8_t count = 0;
	if (getState(x - 1, y - 1)) count++;
	if (getState(x - 1, y)) count++;
	if (getState(x - 1, y + 1)) count++;
	if (getState(x, y - 1)) count++;
	if (getState(x, y + 1)) count++;
	if (getState(x + 1, y - 1)) count++;
	if (getState(x + 1, y)) count++;
	if (getState(x + 1, y + 1)) count++;
	return count;
}

uint32_t Life::getStateHash() {
	uint32_t hash = 0;
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			hash += x * y * (getState(x, y) ? 1 : 0);
		}
	}
	return hash;
}

void Life::clear() {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			setState(x, y, 0);
		}
    }

	flush();
	matrix_write_buffer();
}

pixel_t Life::translate(uint8_t state) {
	pixel_t result;
	if (state > 0) color.h2rgb(this->hue, result);
	return result;
}

void Life::flush() {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			set_pixel(x, y, translate(state[x][y]), OVERLAY_REPLACE);
		}
    }
}

void Life::reset() {
//	srandom(analog_read_p(0) + timer_micros() + timer_millis());

	for (uint8_t i = 0; i < LIFE_HASH_COUNT; i++) {
		this->hashes[i] = 0;
	}
	
	clear();
	
	_delay_ms(100);

	random();
}

void Life::randomize() {
	// random start positions
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			if ((random() & 0x3) == 0x3) {		//25% chance
				setState(x, y, 0x01);
			}
		}
	}
	
	flush();
	matrix_write_buffer();
}