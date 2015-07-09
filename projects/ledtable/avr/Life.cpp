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
	color.setValue(10);
	uint16_t buttons; 
	uint8_t running = 1;
	
	reset();
	
	_delay_ms(255);
	
	
	while (true) {
		for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
			for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
				uint8_t count = getNeighborCount(x, y);
				if (count == 3) {
					// birth
					state[x][y] = 0x01;
				}
				else if (count == 2 || count == 3) {
					// staying alive
					state[x][y] = 0x01;
				}
				else {
					// overpopulation or underpopulation
					state[x][y] = 0x00;
				}
			}
		}

		flush();
		draw_flush();

		//Store board hash
		for (uint8_t i = LIFE_HASH_COUNT - 1; i > 0; i--) {
			hashes[i] = hashes[i - 1];
		}
		hashes[0] = getStateHash();

		uint8_t m = 0;
		for (uint8_t i = 0; i < LIFE_HASH_COUNT; i++) {
			for (uint8_t j = i + 1; j < LIFE_HASH_COUNT; j++) {
				if (hashes[i] == hashes[j]) m++;
			}
		}
		if (m == 0) matches = 0;
		else matches++;

		if (matches >= LIFE_MATCH_COUNT) {
			reset();
			_delay_ms(255);
		}
		
//		void psx_read_gamepad();
//		buttons = psx_buttons();
//		if (buttons & PSB_TRIANGLE) {
//			running = 0;
//		}

		_delay_ms(64);
		color.addHue(1);
	}
}

uint8_t Life::getState(uint8_t x, uint8_t y) {
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0x00;
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
			hash += x * y * (state[x][y] ? 1 : 0);
		}
	}
	return hash;
}

ws2812_t Life::translate(uint8_t state) {
	ws2812_t result;
	if (state > 0) {
		result.red = 5;
		result.green 
	}
	else {
		result.red = 0;
		result.green = 0;
		result.blue = 0;
	}
	return result;
}

void Life::flush() {
    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			draw_set_value(translate(state[x][y]));
			draw_set_pixel(x, y);
		}
    }
}

void Life::reset() {
	for (uint8_t i = 0; i < LIFE_HASH_COUNT; i++) {
		hashes[i] = 0;
	}
	
	// random start positions
	/*
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			if ((random() & 0x3) == 0x3) {		//25% chance
				state[x][y] = 0x01; // birth
			} else {
				state[x][y] = 0x00;
			}
		}
	}
	*/
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			if (x == y) {
				state[x][y] = 0x01; // birth
			} else {
				state[x][y] = 0x00;
			}
		}
	}
	
	flush();
	draw_flush();
}
