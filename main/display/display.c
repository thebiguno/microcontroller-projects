#include "display.h"
#include "../pins.h"
#include "../segment/segment.h"
#include <avr/sfr_defs.h>

#define REG_DIG 0
#define REG_SEG 1
#define REG_ROW 2
#define REG_RED 3
#define REG_GRN 4

static uint8_t digits[] = {SEG_DIG1, SEG_DIG2, SEG_DIG3, SEG_DIG4};
static volatile char segments[4];
static volatile uint8_t flags;
static volatile uint8_t red[8];
static volatile uint8_t grn[8];

void display_set_segments(char _segments[], uint8_t _flags) {
	uint8_t b = 0;
	for (uint8_t i = 0; i < 4; i++) {
		b = segment_lookup(_segments[i]);
		if (!(_flags & _BV(i))) b += SEG_DP;
		segments[i] = b;
	}
	flags = _flags;
}
void display_set_matrix(uint8_t _red[], uint8_t _grn[]) {
	for (uint8_t i = 0; i < 8; i++) {
		red[i] = _red[i];
		grn[i] = _grn[i];
	}
}

uint8_t display_next_shift() {
	static uint8_t reg;
	static uint8_t dig;
	static uint8_t row;
	static uint8_t color;
	uint8_t result = 0;
	switch (reg) {
		case REG_DIG:
			result = digits[dig];
			if (dig == 0) {
				if (flags & _BV(4)) result += SEG_L1L2;
				if (flags & _BV(5)) result += SEG_L3;
			}
			break;
		case REG_SEG:
			result = segments[dig++];
			if (dig == 4) {
				dig = 0;
			}
			break;
		case REG_ROW:
			result = _BV(row);
			color = ~color;
			break;
		case REG_RED:
			result = red[row];
			break;
		case REG_GRN:
			result = grn[row];

			row++;
			if (row > 7) row = 0;
			break;
	}
	reg++;
	if (reg == 5) reg = 0;
	return result;
}

