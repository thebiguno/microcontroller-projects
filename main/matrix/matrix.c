#include "matrix.h"
#include "../shift/shift.h"

#include <avr/sfr_defs.h>

uint8_t bits_set(uint8_t v) {
	uint8_t c = v - ((v >> 1) & 0x55);
	c = ((c >> 0x02) & 0x33) + (c & 0x33);
	c = ((c >> 0x04) + c) & 0x0F;
	return c;
}

void matrix_draw(uint8_t red[], uint8_t green[]) {
	static uint8_t color = 0;
	static uint8_t row = 0;
	static uint8_t dc_r[8];
	static uint8_t dc_g[8];
	
	shift_data(0xFF-_BV(row),2);
	if (color == 0) {
		if (bits_set(red[row]) >= dc_r[row]) {
			shift_data(red[row],3);
		} else {
			shift_data(0x00,3);
		}
		shift_data(0x00,4);
		dc_r[row]++;
		if (dc_r[row] > 7) dc_r[row] = 0;
	} else {
		shift_data(0x00,3);
		if (bits_set(green[row]) >= dc_g[row]) {
			shift_data(green[row],4);
		} else {
			shift_data(0x00,4);
		}
		dc_g[row]++;
		if (dc_g[row] > 7) dc_g[row] = 0;

		row++;
		if (row > 7) row = 0;
	}
	
	color = ~color;
}
