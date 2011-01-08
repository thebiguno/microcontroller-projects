#include "status.h"
#include "../../lib/shift/shift.h"

uint8_t state = 0x00;

void status_init() {
	shift_init(&PORT_LED_SHIFT_DATA, PIN_LED_SHIFT_DATA, &PORT_LED_SHIFT_CLOCK, PIN_LED_SHIFT_CLOCK, &PORT_LED_SHIFT_LATCH, PIN_LED_SHIFT_LATCH);
	shift_out(state);
}

void status_set(uint8_t bv) {
	state |= bv;
	shift_out(state);
}

void status_clear(uint8_t bv) {
	state &= ~bv;
	shift_out(state);
}

void status_toggle(uint8_t bv) {
	state ^= bv;
	shift_out(state);
}