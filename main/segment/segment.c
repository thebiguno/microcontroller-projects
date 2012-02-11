#include "segment.h"

#include <avr/sfr_defs.h>

static volatile uint8_t *_data_port = 0;
static volatile uint8_t *_clock_port = 0;
static volatile uint8_t *_latch_port = 0;

static uint8_t _data_pin = 0;
static uint8_t _clock_pin = 0;
static uint8_t _latch_pin = 0;

static uint8_t lookup(char c) {
	// high = off; so the lookup is defined by what segments are off, not what segments are on
	switch (c) {
		case 0x0: case '0':
		return SEG_G;
		case 0x1: case '1':
		return SEG_A + SEG_D + SEG_E + SEG_F + SEG_G;
		case 0x2: case '2':
		return SEG_C + SEG_F;
		case 0x3: case '3':
		return SEG_E + SEG_F;
		case 0x4: case '4':
		return SEG_A + SEG_D + SEG_E;
		case 0x5: case '5':
		return SEG_B + SEG_E;
		case 0x6: case '6':
		return SEG_B;
		case 0x7: case '7':
		return SEG_D + SEG_E + SEG_F + SEG_G;
		case 0x8: case '8':
		return 0x00;
		case 0x9: case '9':
		return SEG_D + SEG_E;
		case 0xA: case 'a': case 'A':
		return SEG_D;
		case 0xB: case 'b': case 'B':
		return SEG_A + SEG_B;
		case 0xC: case 'c': case 'C':
		return SEG_B + SEG_C + SEG_G;
		case 0xD: case 'd': case 'D':
		return SEG_A + SEG_F;
		case 0xE: case 'e': case 'E':
		return SEG_B + SEG_C;
		case 0xF: case 'f': case 'F':
		return SEG_B + SEG_C + SEG_D;
		case 'g': case 'G':
		return SEG_B + SEG_G;
		case 'h': case 'H':
		return SEG_A + SEG_D;
		case 'j':
		return SEG_A + SEG_E + SEG_F + SEG_G;
		case 'J':
		return SEG_A + SEG_F + SEG_G;
		case 'l': case 'L':
		return SEG_A + SEG_B + SEG_C + SEG_G;
		case 'n':
		return SEG_A + SEG_B + SEG_D + SEG_F;
		case 'N':
		return SEG_D + SEG_G;
		case 'o': case 'O':
		return SEG_A + SEG_B + SEG_F;
		case 'r': case 'R':
		return SEG_A + SEG_B + SEG_C + SEG_D + SEG_F;
		case 't': case 'T':
		return SEG_A + SEG_B + SEG_C;
		case 'u':
		return SEG_A + SEG_B + SEG_F + SEG_G;
		case 'U':
		return SEG_A + SEG_G;
		case 'y': case 'Y':
		return SEG_A + SEG_E;
		case '-':
		return SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F;
		case '_':
		return SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G;
		case '=':
		return SEG_A + SEG_B + SEG_E + SEG_F;
		case '/':
		return SEG_A + SEG_C + SEG_D + SEG_F;
		case '\\':
		return SEG_A + SEG_B + SEG_D + SEG_E;
		
		default: return 0xFF;
	}
}

static void segment_data(uint8_t data) {
	for (int i = 0; i < 8; i++) {
		//Clear the pin first...
		*_data_port &= ~_BV(_data_pin);
		//... then set the bit (if appropriate).  We could probably
		// do this in one step, but this is more clear, plus speed is 
		// (probably) not critical here.
		*_data_port |= (((data >> (7 - i)) & 0x1) << _data_pin);
		
		//Pulse clock to shift in
		*_clock_port &= ~_BV(_clock_pin);
		*_clock_port |= _BV(_clock_pin);
	}
}

static void segment_latch() {
	*_latch_port &= ~_BV(_latch_pin);
	*_latch_port |= _BV(_latch_pin);
}

void segment_init(volatile uint8_t *data_port, uint8_t data_pin, volatile uint8_t *clock_port, uint8_t clock_pin, volatile uint8_t *latch_port, uint8_t latch_pin){
	_data_port = data_port;
	_clock_port = clock_port;
	_latch_port = latch_port;

	*(_data_port - 0x1) |= _BV(data_pin);
	*(_clock_port - 0x1) |= _BV(clock_pin);
	*(_latch_port - 0x1) |= _BV(latch_pin);
	
	_data_pin = data_pin;
	_clock_pin = clock_pin;
	_latch_pin = latch_pin;
}

void segment_draw(char c[], uint8_t flags) {
	uint8_t b = SEG_DIG1;

	if (!bit_is_set(flags,4)) b += SEG_L1L2;
	if (!bit_is_set(flags,5)) b += SEG_L3;

	segment_data(b);
	b = lookup(c[0]);
	if (!bit_is_set(flags,0)) b += SEG_DP;
	segment_data(b);
	segment_latch();
	
	b = SEG_DIG2;
	segment_data(b);
	b = lookup(c[1]);
	if (!bit_is_set(flags,1)) b += SEG_DP;
	segment_data(b);
	segment_latch();
	
	b = SEG_DIG3;
	segment_data(b);
	b = lookup(c[2]);
	if (!bit_is_set(flags,2)) b += SEG_DP;
	segment_data(b);
	segment_latch();
	
	b = SEG_DIG4;
	segment_data(b);
	b = lookup(c[3]);
	if (!bit_is_set(flags,3)) b += SEG_DP;
	segment_data(b);
	segment_latch();
	
	segment_data(0x00);
	segment_data(0xFF);
	segment_latch();
}
