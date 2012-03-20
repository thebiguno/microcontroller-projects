#include "display.h"
#include "../pins.h"
#include "../segment/segment.h"
#include <avr/sfr_defs.h>

#define REG_DIG 0
#define REG_SEG 1

static uint8_t digits[] = {SEG_DIG1, SEG_DIG2, SEG_DIG3, SEG_DIG4};
static volatile char segments[4];
static volatile uint8_t flags;

void display_set_segments(uint8_t _segments[], uint8_t _flags) {
	for (uint8_t i = 0; i < 4; i++) {
		segments[i] = _segments[i];
	}
	flags = _flags;
}
uint8_t display_next_shift() {
	static uint8_t reg;
	static uint8_t dig;
	uint8_t result = 0;
	switch (reg) {
		case REG_DIG:
			result = digits[dig];
			// if (dig == 0) {
			// 	if (flags & _BV(0)) result += SEG_L1L2;
			// 	if (flags & _BV(1)) result += SEG_L3;
			// }
			break;
		case REG_SEG:
			result = segments[dig++];
			if (dig == 4) {
				dig = 0;
			}
			break;
	}
	reg++;
	if (reg == 2) reg = 0;
	return result;
}

