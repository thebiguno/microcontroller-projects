#include "matrix.h"
#include "../shift/shift.h"

#include <avr/sfr_defs.h>

#define REG_ROW 2
#define REG_RED 3
#define REG_GRN 4

/* see http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel */
uint8_t bits_set(uint8_t v) {
	uint8_t c = v - ((v >> 1) & 0x55);
	c = ((c >> 0x02) & 0x33) + (c & 0x33);
	c = ((c >> 0x04) + c) & 0x0F;
	return c;
}

void matrix_draw(uint8_t red[], uint8_t grn[]) {
	for (uint8_t dc = 0; dc < 8; dc++) {
		for (uint8_t row = 0; row < 8; row++) {
			uint8_t j = row + (dc * 8);
			shift_data(REG_ROW, j, 0xFF-_BV(row));
			if (bits_set(red[row]) >= dc) {
				shift_data(REG_RED, j, red[row]);
			} else {
				shift_data(REG_RED, j, 0x00);
			}
			if (bits_set(grn[row]) >= dc) {
				shift_data(REG_GRN, j, grn[row]);
			} else {
				shift_data(REG_GRN, j, 0x00);
			}
		}
	}
}
