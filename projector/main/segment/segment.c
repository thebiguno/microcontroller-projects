#include "segment.h"
#include "../pins.h"
#include <avr/sfr_defs.h>

/*
 * high = off; so the lookup is defined by what segments are off, not what segments are on
 */

uint8_t segment_character(char c) {
	switch (c) {
		case ' ':
		return SEG_DP + SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G;
		case '/':
		return SEG_DP + SEG_A + SEG_C + SEG_D + SEG_F;
		case '0':
		return SEG_DP + SEG_G;
		case '1':
		return SEG_DP + SEG_A + SEG_D + SEG_E + SEG_F + SEG_G;
		case '2':
		return SEG_DP + SEG_C + SEG_F;
		case '3':
		return SEG_DP + SEG_E + SEG_F;
		case '4':
		return SEG_DP + SEG_A + SEG_D + SEG_E;
		case '5':
		return SEG_DP + SEG_B + SEG_E;
		case '6':
		return SEG_DP + SEG_B;
		case '7':
		return SEG_DP + SEG_D + SEG_E + SEG_F + SEG_G;
		case '8':
		return SEG_DP;
		case '9':
		return SEG_DP + SEG_D + SEG_E;
		case 'a': case 'A':
		return SEG_DP + SEG_D;
		case 'b': case 'B':
		return SEG_DP + SEG_A + SEG_B;
		case 'c': case 'C':
		return SEG_DP + SEG_B + SEG_C + SEG_G;
		case 'd': case 'D':
		return SEG_DP + SEG_A + SEG_F;
		case 'e': case 'E':
		return SEG_DP + SEG_B + SEG_C;
		case 'f': case 'F':
		return SEG_DP + SEG_B + SEG_C + SEG_D;
		case 'g': case 'G':
		return SEG_DP + SEG_B + SEG_G;
		case 'h': case 'H':
		return SEG_DP + SEG_A + SEG_B + SEG_D;
		case 'i': case 'I':
		return SEG_DP + SEG_A + SEG_B + SEG_C + SEG_D + SEG_G;
		case 'j': case 'J':
		return SEG_DP + SEG_A + SEG_F + SEG_G;
		case 'k': case 'K':
		return SEG_DP + SEG_B + SEG_D;
		case 'l': case 'L':
		return SEG_DP + SEG_A + SEG_B + SEG_C + SEG_G;
		case 'm': case 'M':
		return SEG_DP + SEG_B + SEG_D + SEG_F + SEG_G;
		case 'n': case 'N':
		return SEG_DP + SEG_D + SEG_G;
		case 'o': case 'O':
		return SEG_DP + SEG_A + SEG_B + SEG_F;
		case 'p': case 'P':
		return SEG_DP + SEG_C + SEG_D;
		case 'q': case 'Q':
		return SEG_DP + SEG_C + SEG_E;
		case 'r': case 'R':
		return SEG_DP + SEG_C + SEG_D + SEG_G;
		case 's': case 'S':
		return SEG_DP + SEG_A + SEG_B + SEG_E;
		case 't': case 'T':
		return SEG_DP + SEG_A + SEG_B + SEG_C;
		case 'u': case 'U': case 0x1F: case 'v': case 'V':
		return SEG_DP + SEG_A + SEG_G;
		case 'x': case 'X':
		return SEG_DP + SEG_A + SEG_D;
		case 'y': case 'Y':
		return SEG_DP + SEG_A + SEG_E;
		case '^':
		return SEG_DP + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G;
		case '-':
		return SEG_DP + SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F;
		case '_':
		return SEG_DP + SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G;
		case '=':
		return SEG_DP + SEG_A + SEG_B + SEG_E + SEG_F;
		case '\\':
		return SEG_DP + SEG_A + SEG_B + SEG_D + SEG_E;
		
		default: 
		return 0xFF;
	}
}

uint8_t segment_decimal(uint8_t v) {
	static uint8_t table[12] = {
		SEG_DP + SEG_G,
		SEG_DP + SEG_A + SEG_D + SEG_E + SEG_F + SEG_G,
		SEG_DP + SEG_C + SEG_F,
		SEG_DP + SEG_E + SEG_F,
		SEG_DP + SEG_A + SEG_D + SEG_E,
		SEG_DP + SEG_B + SEG_E,
		SEG_DP + SEG_B,
		SEG_DP + SEG_D + SEG_E + SEG_F + SEG_G,
		SEG_DP,
		SEG_DP + SEG_D + SEG_E
	};
	return table[v];
}

uint8_t segment_dp(uint8_t s) {
	return s - SEG_DP;
}
