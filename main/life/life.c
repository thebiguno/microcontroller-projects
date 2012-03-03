#include "life.h"
#include <stdlib.h>
#include <avr/io.h>

static uint8_t rows = 8;
static uint8_t cols = 8;
static uint8_t curr_state[8][8];

void life_update() {
	static uint8_t new_state[8][8];
	
	for (uint8_t i = 0; i < rows; i++) {
		for (uint8_t j = 0; j < cols; j++) {
			new_state[i][j] = 0;
			// count up the neighborhood
			uint8_t ct = 0;
			if (i > 0 && i < rows - 1) {
				ct += curr_state[i-1][j] > 0 ? 1 : 0; //n
				ct += curr_state[i+1][j] > 0 ? 1 : 0; //s
			}
			if (j > 0 && j < cols - 1) {
				ct += curr_state[i][j-1] > 0 ? 1 : 0; //w
				ct += curr_state[i][j+1] > 0 ? 1 : 0; //e
			}
			if (i > 0 && j > 0) {
				ct += curr_state[i-1][j-1] > 0 ? 1 : 0; //nw
			}
			if (i > 0 && j < cols - 1) {
				ct += curr_state[i-1][j+1] > 0 ? 1 : 0; //ne
			}
			if (i < rows - 1 && j > 0) {
				ct += curr_state[i+1][j-1] > 0 ? 1 : 0; //sw
			}
			if (i < rows - 1 && j < cols - 1) {
				ct += curr_state[i+1][j+1] > 0 ? 1 : 0; //se
			}

			// set the new state
			if (curr_state[i][j] == 0 && ct == 3) { // birth
				new_state[i][j] = 1;
			} else if (curr_state[i][j] > 0 && ct == 2) { // alive 2
				new_state[i][j] = 2;
			} else if (curr_state[i][j] > 0 && ct == 3) { // alive 3
				new_state[i][j] = 3;
			} else { // death
				new_state[i][j] = 0;
			}
		}
	}
	
	// copy the new state to the current state
	for (uint8_t i = 0; i < rows; i++) {
		for (uint8_t j = 0; j < cols; j++) {
			curr_state[i][j] = new_state[i][j];
		}
	}
}

void life_randomize() {
	for (uint8_t i = 0; i < rows; i++) {
		for (uint8_t j = 0; j < cols; j++) {
			curr_state[i][j] = (rand() & 0x4000) == 0x4000 ? 1 : 0;
		}
	}
}

void life_matrix(uint8_t red[], uint8_t grn[]) {
	for (uint8_t i = 0; i < rows; i++) {
		red[i] = 0x00;
		grn[i] = 0x00;
		for (uint8_t j = 0; j < cols; j++) {
			if (curr_state[i][j] == 1) { // birth is green
				grn[i] |= _BV(j);
			} else if (curr_state[i][j] == 2) { // alive 2 is red
				red[i] |= _BV(j);
			} else if (curr_state[i][j] == 3) { // alive 3 is yellow
				grn[i] |= _BV(j);
				red[i] |= _BV(j);
			}
		}
	}
}