#include "clock.h"

#include <avr/sfr_defs.h>

/*
 * This is the normal method of keeping time used worldwide
 * Drawn in the top-left corner.
 */
void clock_traditional(uint32_t ms) {
	//milliseconds to traditional (24:59:59)
	uint8_t hr = ms / 3600000;	// 1/24 day (hour)
	ms -= 3600000 * (uint32_t) hr;
	uint8_t mn = ms / 60000;	// 1/1440 day (minute)
	ms -= 60000 * (uint32_t) mn;
	uint8_t sc = ms / 1000;		// 1/86400 day (second)	
	
	uint8_t digits[6];
	digits[0] = 0;
	while (hr > 9) {
		digits[0] += 1;
		hr -= 10;
	}
	digits[1] = hr;
	
	digits[2] = 0;
	while (mn > 9) {
		digits[2] += 1;
		mn -= 10;
	}
	digits[3] = mn;
	
	digits[4] = 0;
		while(sc > 9) {
		digits[4] += 1;
		sc -= 10;
	}
	digits[5] = sc;

	for (uint8_t i = 0; i < 6; i++) { // digits
		// build a 1x6 bar
		uint8_t v = digits[i];
		uint8_t col = OFFSET_TRAD_X;
		for (uint8_t j = 0; j < 8; j++) {
			if (j == 2 || j == 5) col++; // blank columns
			if (_BV(v) & 0x01) {
				set_pixel(col, OFFSET_TRAD_Y + 7, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_TRAD_Y + 6, RED_3, OVERLAY_REPLACE);
			}
			if (_BV(v) & 0x02) {
				set_pixel(col, OFFSET_TRAD_Y + 5, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_TRAD_Y + 4, RED_3, OVERLAY_REPLACE);
			}
			if (_BV(v) & 0x04) {
				set_pixel(col, OFFSET_TRAD_Y + 3, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_TRAD_Y + 2, RED_3, OVERLAY_REPLACE);
			}
			if (_BV(v) & 0x08) {
				set_pixel(col, OFFSET_TRAD_Y + 1, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_TRAD_Y + 0, RED_3, OVERLAY_REPLACE);
			}
		}
	}
}

/*
 * This method was used by the ancient Mayans
 */
void clock_vigesimal(uint32_t ms) {
	uint8_t digits[4];
	//milliseconds to vigesimal (19.19.19.19)
	digits[0] = ms / 4320000;	// 1/20 day = 1 h 12 m
	ms -= 4320000 * (uint32_t) digits[0];
	digits[1] = ms / 216000;	// 1/400 day = 3 m 36 s
	ms -= 216000 * (uint32_t) digits[1];
	digits[2] = ms / 10800;		// 1/8000 day = 10.8 s
	ms -= 10800 * (uint32_t) digits[2];
	digits[3] = ms / 540;		// 1/160000 day = .54 s
	
	for (uint8_t i = 0; i < 4; i++) { // digits
		// build a 4x4 mayan number blocks
	
		uint8_t v = digits[i];
		uint8_t row = OFFSET_MAYAN_Y;
		if (i > 1) row += 4; // move blocks c and d down four rows
	
		// draw the dots
		uint8_t col = OFFSET_MAYAN_X + (i % 2 == 1) ? 4 : 0; // move blocks b and d over 4 cols
		if (v % 5 > 0) set_pixel(col + 3, row, RED_3, OVERLAY_REPLACE);
		if (v % 5 > 1) set_pixel(col + 2, row, RED_3, OVERLAY_REPLACE);
		if (v % 5 > 2) set_pixel(col + 1, row, RED_3, OVERLAY_REPLACE);
		if (v % 5 > 3) set_pixel(col + 0, row, RED_3, OVERLAY_REPLACE);
	
		// draw the lines
		if (v > 4) draw_line(col, row + 3, col + 4, row + 3, RED_3, OVERLAY_REPLACE);
		if (v > 9) draw_line(col, row + 2, col + 4, row + 2, RED_3, OVERLAY_REPLACE);
		if (v > 14) draw_line(col, row + 1, col + 4, row + 1, RED_3, OVERLAY_REPLACE);
		
		// TODO draw the dots on first blank line in a different color instead of always on the top line
	}
}

/*
 * This method was first proposed in the 1850s by John W. Nystrom
 */
void clock_hexadecimal(uint32_t ms) {
	uint8_t digits[4];
	//milliseconds to hexadecimal (F_FF_F)
	digits[0] = ms / 5400000;		// 1/16 day (hex hour) = 1 h 30 m
	ms -= 5400000 * (uint32_t) digits[0];
	digits[1] = ms / 337500;		// 1/256 day (hex maxime) = 5 m 37.5 s
	ms -= 337500 * (uint32_t) digits[1];
	ms *= 100;						// bump up the precision
	digits[2] = ms / 2109375;		// 1/4096 day (hex minute) ~= 21 seconds
	ms -= 2109375 * (uint32_t) digits[2];
	ms *= 100;						// bump up the precision again
	digits[3] = ms / 13183593;		// 1/65536 day (hex second) ~= 1.32 seconds
	
	for (uint8_t i = 0; i < 5; i++) { // digits
		uint8_t v = digits[i];
		for (uint8_t col = 0; col < 8; col = col + 2) {
			if (_BV(v) & 0x01) {
				set_pixel(col, OFFSET_HEX_Y + 7, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_HEX_Y + 6, RED_3, OVERLAY_REPLACE);
				set_pixel(col + 1, OFFSET_HEX_Y + 7, RED_3, OVERLAY_REPLACE); 
				set_pixel(col + 1, OFFSET_HEX_Y + 6, RED_3, OVERLAY_REPLACE);
			}
			if (_BV(v) & 0x02) {
				set_pixel(col, OFFSET_HEX_Y + 5, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_HEX_Y + 4, RED_3, OVERLAY_REPLACE);
				set_pixel(col + 1, OFFSET_HEX_Y + 5, RED_3, OVERLAY_REPLACE); 
				set_pixel(col + 1, OFFSET_HEX_Y + 4, RED_3, OVERLAY_REPLACE);
			}
			if (_BV(v) & 0x04) {
				set_pixel(col, OFFSET_HEX_Y + 3, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_HEX_Y + 2, RED_3, OVERLAY_REPLACE);
				set_pixel(col + 1, OFFSET_HEX_Y + 3, RED_3, OVERLAY_REPLACE); 
				set_pixel(col + 1, OFFSET_HEX_Y + 2, RED_3, OVERLAY_REPLACE);
			}
			if (_BV(v) & 0x08) {
				set_pixel(col, OFFSET_HEX_Y + 1, RED_3, OVERLAY_REPLACE); 
				set_pixel(col, OFFSET_HEX_Y + 0, RED_3, OVERLAY_REPLACE);
				set_pixel(col + 1, OFFSET_HEX_Y + 1, RED_3, OVERLAY_REPLACE); 
				set_pixel(col + 1, OFFSET_HEX_Y + 0, RED_3, OVERLAY_REPLACE);
			}
		}
	}
}

// void clock_dozenal(uint32_t ms) {
// 	uint8_t digits[5];
// 	//milliseconds to decimal (BBB.BB)
// 	digits[0] = ms / 7200000;	// 1/12 day = 2 h
// 	ms -= 7200000 * (uint32_t) digits[0];
// 	digits[1] = ms / 600000;	// 1/144 day = 10 m
// 	ms -= 600000 * (uint32_t) digits[1];
// 	digits[2] = ms / 50000;		// 1/1728 day = 50 s
// 	ms -= 50000 * (uint32_t) digits[2];
// 	digits[3] = ms / 4167;		// 1/20736 day ~= 4.167 s
// 	ms -= 4167 * (uint32_t) digits[3];
// 	digits[4] = ms / 347;		// 1/248832 day ~= .347 s
// 	
// 	for (uint8_t i = 0; i < 4; i++) {
// 		_segments[i] = segment_decimal(digits[i]);
// 	}
// 	if (digits[0] == 0) {
// 		_segments[0] = segment_character(' ');
// 		if (digits[1] == 0) {
// 			_segments[1] = segment_character(' ');
// 		}
// 	}
// 	
// 	// add the decimal place
// 	_segments[2] = segment_dp(_segments[2]);
// 
// 	// flash the extra dot for digit 5
// 	if ((digits[4] & _BV(0)) == _BV(0)) {
// 		_segment_flags = _BV(1);
// 	} else {
// 		_segment_flags = 0x00;
// 	}
// 	
// 	clock_clear_matrix();
// 
// 	// draws 3x3 dice patterns in each cornern
// 	// draws a flashing dot in the middle for centibeats
// 	for (uint8_t i = 0; i < 4; i++) { // segments (rows)
// 		uint8_t v = digits[i];
// 		int row = 0;
// 		if (i > 1) row += 4; // move down four rows
// 		if (i == 1 || i == 2) { // make green instead of red
// 			uint8_t sh = (i == 1) ? 4 : 0; // shift over three cols
// 			if (v == 0) {
// 				_matrix_grn[row+0] = (0x04 << sh);
// 			} else if (v == 1) {
// 				_matrix_grn[row+0] = (0x08 << sh);
// 			} else if (v == 2) {
// 				_matrix_grn[row+1] = (0x08 << sh);
// 			} else if (v == 3) {
// 				_matrix_grn[row+2] = (0x08 << sh);
// 			} else if (v == 4) {
// 				_matrix_grn[row+3] = (0x08 << sh);
// 			} else if (v == 5) {
// 				_matrix_grn[row+3] = (0x04 << sh);
// 			} else if (v == 6) {
// 				_matrix_grn[row+3] = (0x02 << sh);
// 			} else if (v == 7) {
// 				_matrix_grn[row+3] = (0x01 << sh);
// 			} else if (v == 8) {
// 				_matrix_grn[row+2] = (0x01 << sh);
// 			} else if (v == 9) {
// 				_matrix_grn[row+1] = (0x01 << sh);
// 			} else if (v == 10) {
// 				_matrix_grn[row+0] = (0x01 << sh);
// 			} else if (v == 11) {
// 				_matrix_grn[row+0] = (0x02 << sh);
// 			}
// 		} else {
// 			uint8_t sh = (i == 3) ? 4 : 0; // shift over 4 cols
// 			if (v == 0) {
// 				_matrix_red[row+0] = (0x04 << sh);
// 			} else if (v == 1) {
// 				_matrix_red[row+0] = (0x08 << sh);
// 			} else if (v == 2) {
// 				_matrix_red[row+1] = (0x08 << sh);
// 			} else if (v == 3) {
// 				_matrix_red[row+2] = (0x08 << sh);
// 			} else if (v == 4) {
// 				_matrix_red[row+3] = (0x08 << sh);
// 			} else if (v == 5) {
// 				_matrix_red[row+3] = (0x04 << sh);
// 			} else if (v == 6) {
// 				_matrix_red[row+3] = (0x02 << sh);
// 			} else if (v == 7) {
// 				_matrix_red[row+3] = (0x01 << sh);
// 			} else if (v == 8) {
// 				_matrix_red[row+2] = (0x01 << sh);
// 			} else if (v == 9) {
// 				_matrix_red[row+1] = (0x01 << sh);
// 			} else if (v == 10) {
// 				_matrix_red[row+0] = (0x01 << sh);
// 			} else if (v == 11) {
// 				_matrix_red[row+0] = (0x02 << sh);
// 			}
// 		}
// 	}
// }
// 
// /*
//  * This method was introduced during the French Revolution in 1793
//  * It's also equivalent to Swatch time
//  */
// void clock_decimal(uint32_t ms) {
// 	uint8_t digits[5];
// 	//milliseconds to decimal (999.99)
// 	digits[0] = ms / 8640000;	// 1/10 day (deciday) = 2 h 24 m
// 	ms -= 8640000 * (uint32_t) digits[0];
// 	digits[1] = ms / 864000;	// 1/100 day (centiday) = 14 m 24 s
// 	ms -= 864000 * (uint32_t) digits[1];
// 	digits[2] = ms / 86400;		// 1/1000 day (milliday; beat) = 1 m 26.4 s
// 	ms -= 86400 * (uint32_t) digits[2];
// 	digits[3] = ms / 8640;		// 1/10000 day (decibeat)= 8.64 s
// 	ms -= 8640 * (uint32_t) digits[3];
// 	digits[4] = ms / 864;		// 1/100000 day (centibeat) / .864 s
// 	
// 	for (uint8_t i = 0; i < 4; i++) {
// 		_segments[i] = segment_decimal(digits[i]);
// 	}
// 	if (digits[0] == 0) {
// 		_segments[0] = segment_character(' ');
// 		if (digits[1] == 0) {
// 			_segments[1] = segment_character(' ');
// 		}
// 	}
// 	
// 	// add the decimal place
// 	_segments[2] = segment_dp(_segments[2]);
// 
// 	// flash the extra dot for digit 5
// 	if ((digits[4] & _BV(0)) == _BV(0)) {
// 		_segment_flags = _BV(1);
// 	} else {
// 		_segment_flags = 0x00;
// 	}	
// 	
// 	clock_clear_matrix();
// 
// 	// draws 3x3 dice patterns in each cornern
// 	// draws a flashing dot in the middle for centibeats
// 	for (uint8_t i = 0; i < 4; i++) { // segments (rows)
// 		uint8_t v = digits[i];
// 		int row = 2;
// 		if (i > 1) row += 5; // move md and ud down five rows
// 		if (i == 1 || i == 2) { // make cd and md green instead of red
// 			uint8_t sh = (i == 1) ? 5 : 0; // shift cd over three cols
// 			if (v == 1) {
// 				_matrix_grn[row-1] = (0x02 << sh);
// 			} else if (v == 2) {
// 				_matrix_grn[row-0] = (0x01 << sh);
// 				_matrix_grn[row-2] = (0x04 << sh);
// 			} else if (v == 3) {
// 				_matrix_grn[row-0] = (0x01 << sh);
// 				_matrix_grn[row-1] = (0x02 << sh);
// 				_matrix_grn[row-2] = (0x04 << sh);
// 			} else if (v == 4) {
// 				_matrix_grn[row-0] = (0x05 << sh);
// 				_matrix_grn[row-2] = (0x05 << sh);
// 			} else if (v == 5) {
// 				_matrix_grn[row-0] = (0x05 << sh);
// 				_matrix_grn[row-1] = (0x02 << sh);
// 				_matrix_grn[row-2] = (0x05 << sh);
// 			} else if (v == 6) {
// 				_matrix_grn[row-0] = (0x05 << sh);
// 				_matrix_grn[row-1] = (0x05 << sh);
// 				_matrix_grn[row-2] = (0x05 << sh);
// 			} else if (v == 7) {
// 				_matrix_grn[row-0] = (0x05 << sh);
// 				_matrix_grn[row-1] = (0x07 << sh);
// 				_matrix_grn[row-2] = (0x05 << sh);
// 			} else if (v == 8) {
// 				_matrix_grn[row-0] = (0x07 << sh);
// 				_matrix_grn[row-1] = (0x05 << sh);
// 				_matrix_grn[row-2] = (0x07 << sh);
// 			} else if (v == 9) {
// 				_matrix_grn[row-0] = (0x07 << sh);
// 				_matrix_grn[row-1] = (0x07 << sh);
// 				_matrix_grn[row-2] = (0x07 << sh);
// 			}
// 		} else {
// 			uint8_t sh = (i == 3) ? 5 : 0; // shift ud over three cols
// 			if (v == 1) {
// 				_matrix_red[row-1] = (0x02 << sh);
// 			} else if (v == 2) {
// 				_matrix_red[row-0] = (0x01 << sh);
// 				_matrix_red[row-2] = (0x04 << sh);
// 			} else if (v == 3) {
// 				_matrix_red[row-0] = (0x01 << sh);
// 				_matrix_red[row-1] = (0x02 << sh);
// 				_matrix_red[row-2] = (0x04 << sh);
// 			} else if (v == 4) {
// 				_matrix_red[row-0] = (0x05 << sh);
// 				_matrix_red[row-2] = (0x05 << sh);
// 			} else if (v == 5) {
// 				_matrix_red[row-0] = (0x05 << sh);
// 				_matrix_red[row-1] = (0x02 << sh);
// 				_matrix_red[row-2] = (0x05 << sh);
// 			} else if (v == 6) {
// 				_matrix_red[row-0] = (0x05 << sh);
// 				_matrix_red[row-1] = (0x05 << sh);
// 				_matrix_red[row-2] = (0x05 << sh);
// 			} else if (v == 7) {
// 				_matrix_red[row-0] = (0x05 << sh);
// 				_matrix_red[row-1] = (0x07 << sh);
// 				_matrix_red[row-2] = (0x05 << sh);
// 			} else if (v == 8) {
// 				_matrix_red[row-0] = (0x07 << sh);
// 				_matrix_red[row-1] = (0x05 << sh);
// 				_matrix_red[row-2] = (0x07 << sh);
// 			} else if (v == 9) {
// 				_matrix_red[row-0] = (0x07 << sh);
// 				_matrix_red[row-1] = (0x07 << sh);
// 				_matrix_red[row-2] = (0x07 << sh);
// 			}
// 		}
// 	}
// 
// 	uint8_t nd = digits[4];
// 	if (nd == 1 || nd == 4 || nd == 7 || nd == 3 || nd == 6 || nd == 9) {
// 		_matrix_red[3] = 0x18;
// 		_matrix_red[4] = 0x18;
// 	}
// 	if (nd == 2 || nd == 5 || nd == 8 || nd == 3 || nd == 6 || nd == 9) {
// 		_matrix_grn[3] = 0x18;
// 		_matrix_grn[4] = 0x18;
// 	}
// }

// void clock_octal(uint32_t ms) {
// 	//milliseconds to octal (777.777)
// 	uint8_t digits[6];
// 	digits[0] = ms / 10800000;		// 1/8 day = 3 h
// 	ms -= 10800000 * (uint32_t) digits[0] ;
// 	digits[1] = ms / 1350000;		// 1/64 day = 22 m 30 s
// 	ms -= 1350000 * (uint32_t) digits[1];
// 	digits[2] = ms / 168750;		// 1/512 day ~= 2 m 49 s
// 	ms -= 168750 * (uint32_t) digits[2];
// 	ms *= 100;						// bump up the precision
// 	digits[3] = ms / 2109375;		// 1/4096 day ~= 21 s
// 	ms -= 2109375 * (uint32_t) digits[3];
// 	ms *= 100;						// bump up the precision again
// 	digits[4] = ms / 26367187;		// 1/32768 day ~= 2.63 s
// 	ms -= 26367187 * (uint32_t) digits[4];
// 	ms *= 100;						// bump up the precision again
// 	digits[5] = ms / 329589843;		// 1/262144 day ~= .329 s
// 
// 	for (uint8_t i = 0; i < 4; i++) {
// 		_segments[i] = segment_decimal(digits[i]);
// 	}
// 	if (digits[0] == 0) {
// 		_segments[0] = segment_character(' ');
// 		if (digits[1] == 0) {
// 			_segments[1] = segment_character(' ');
// 		}
// 	}
// 	
// 	// add the decimal place
// 	_segments[2] = segment_dp(_segments[2]);
// 
// 	// flash the extra dot for digit 5
// 	if ((digits[4] & _BV(0)) == _BV(0)) {
// 		_segment_flags = _BV(1);
// 	} else {
// 		_segment_flags = 0x00;
// 	}
// 
// 	clock_clear_matrix();
// 
// 	// 1x2 pixels for each bit, a on top, f on bottom
// 	for (uint8_t i = 0; i < 6; i++) { // segments (rows)
// 		// build a 1x6 bar
// 		uint8_t v = _segments[i];
// 		for (uint8_t j = 0; j < 3; j++) { // bits (cols)
// 			if ((v & _BV(j)) != 0) {
// 				if (i == 1 || i == 3 || i == 5) {
// 					_matrix_grn[i+1] |= 2 << (5-(j*2));
// 					_matrix_grn[i+1] |= 2 << (4-(j*2));
// 				} else {
// 					_matrix_red[i+1] |= 2 << (5-(j*2));
// 					_matrix_red[i+1] |= 2 << (4-(j*2));
// 				}
// 			}
// 		}
// 	}
// }

/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_draw(uint32_t ms) {
	draw_rectangle(0,0, 23,15, DRAW_FILLED, 0x00, OVERLAY_REPLACE);
	
	clock_traditional(ms);
	clock_vigesimal(ms);
	clock_hexadecimal(ms);
}


