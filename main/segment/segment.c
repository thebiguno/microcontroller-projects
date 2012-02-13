#include "segment.h"
#include "../pins.h"
#include "../shift/shift.h"
#include <avr/sfr_defs.h>

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
		case 0x10: case 'g': case 'G':
		return SEG_B + SEG_G;
		case 0x11: case 'h': case 'H':
		return SEG_A + SEG_D;
		case 0x12: case 'i': case 'I':
		return SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G;
		case 0x13: case 'j':
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

void segment_draw(char c[], uint8_t flags) {
	static uint8_t seg = 0;

	uint8_t b = 0;
	if (seg == 0) {
		b = SEG_DIG1;

		if (flags & _BV(4)) b += SEG_L1L2;
		if (flags & _BV(5)) b += SEG_L3;

		shift_data(b);
		b = lookup(c[0]);
		if (!(flags & _BV(0))) b += SEG_DP;
		shift_data(b);
	} else if (seg == 1) {
		b = SEG_DIG2;
		shift_data(b);
		b = lookup(c[1]);
		if (!(flags & _BV(1))) b += SEG_DP;
		shift_data(b);
	} else if (seg == 2) {
		b = SEG_DIG3;
		shift_data(b);
		b = lookup(c[2]);
		if (!(flags & _BV(2))) b += SEG_DP;
		shift_data(b);
	} else if (seg == 3) {	
		b = SEG_DIG4;
		shift_data(b);
		b = lookup(c[3]);
		if (!(flags & _BV(3))) b += SEG_DP;
		shift_data(b);
	} else {
		shift_data(0x00);
		shift_data(0xFF);
	}
	
	seg++;
	if (seg > 3 + 32) {
		seg = 0;
	}
}
