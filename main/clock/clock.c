#include "clock.h"

#include <avr/sfr_defs.h>

static volatile uint8_t _mode = 0;
static char _segments[6] = {0,0,0,0,0,0};
static uint8_t _segment_flags = 0;
static uint8_t _matrix_red[8] = {0,0,0,0,0,0,0,0};
static uint8_t _matrix_grn[8] = {0,0,0,0,0,0,0,0};

void clock_mode(uint8_t mode) {
	_mode = mode;
}

/*
 * Sets a char array according to the time in the given mode.
 */
void clock_segments(char result[]) {
	for (uint8_t i = 0; i < 4; i++) {
		result[i] = _segments[i];
	}
}

uint8_t clock_segment_flags() {
	return _segment_flags;
}


/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_matrix(uint8_t red[], uint8_t grn[]) {
	for (uint8_t i = 0; i < 8; i++) {
		red[i] = _matrix_red[i];
		grn[i] = _matrix_grn[i];
	}
}

void clock_clear_matrix() {
	for (uint8_t i = 0; i < 8; i++) {
		_matrix_red[i] = 0;
		_matrix_grn[i] = 0;
	}
}

/*
 * This is the normal method of keeping time used worldwide
 */
void clock_traditional(uint32_t ms) {
	//milliseconds to traditional (24:59:59)
	uint8_t hr = ms / 3600000;	// 1/24 day (hour)
	ms -= 3600000 * (uint32_t) hr;
	uint8_t mn = ms / 60000;	// 1/1440 day (minute)
	ms -= 60000 * (uint32_t) mn;
	uint8_t sc = ms / 1000;		// 1/86400 day (second)	
	
	_segments[0] = 0;
	while (hr > 9) {
		_segments[0] += 1;
		hr -= 10;
	}
	_segments[1] = hr;
	
	_segments[2] = 0;
	while (mn > 9) {
		_segments[2] += 1;
		mn -= 10;
	}
	_segments[3] = mn;
	
	_segments[4] = 0;
		while(sc > 9) {
		_segments[4] += 1;
		sc -= 10;
	}
	_segments[5] = sc;

	_segment_flags = 0;
	if ((sc & _BV(0)) == _BV(0)) {
		_segment_flags |= _BV(4);
	}
	
	clock_clear_matrix();

	// 3 2x8 bars with a space sparating them, hr on top, sec on bottom
	for (int i = 0; i < 6; i++) { // segments (rows)
		// build a 1x6 bar
		int v = _segments[i];
		for (int j = 0; j < 8; j++) { // bits (cols)
			uint8_t row = i;
			if (i > 1) row++; // blank rows
			if (i > 3) row++;
			if ((v & _BV(j)) != 0) {
				if (i == 2 || i == 3) {
					_matrix_grn[row] |= 1 << (7-(j*2));
					_matrix_grn[row] |= 1 << (6-(j*2));
				} else {
					_matrix_red[row] |= 1 << (7-(j*2));
					_matrix_red[row] |= 1 << (6-(j*2));
				}
			}
		}
	}
	
	if (_segments[0] == 0) {
		_segments[0] = ' ';
	}
}

/*
 * This method was used by the ancient Mayans
 */
void clock_vigesimal(uint32_t ms) {
	//milliseconds to vigesimal (19.19.19.19)
	uint8_t a = ms / 4320000;	// 1/20 day = 1 h 12 m
	ms -= 4320000 * (uint32_t) a;
	uint8_t b = ms / 216000;	// 1/400 day = 3 m 36 s
	ms -= 216000 * (uint32_t) b;
	uint8_t c = ms / 10800;		// 1/8000 day = 10.8 s
	ms -= 10800 * (uint32_t) c;
	uint8_t d = ms / 540;		// 1/160000 day = .54 s
	
	_segments[0] = a;
	_segments[1] = b;
	_segments[2] = c;
	_segments[3] = d;
	
	_segment_flags = 0x00;
	
	clock_clear_matrix();

	for (uint8_t i = 0; i < 4; i++) { // segments (rows)
		// build a 4x4 mayan number square
	
		uint8_t v = _segments[i];
		int row = 3;
		if (v > 4) row--;
		if (v > 9) row--;
		if (v > 14) row--;
		if (i > 1) row += 4; // mode c and d down four rows
	
		// draw the dots
		uint8_t sh = (i % 2 == 1) ? 4 : 0; // shift b and d over 4 cols
		if (v % 5 > 0) _matrix_red[row] |= (0x01 << sh);
		if (v % 5 > 1) _matrix_red[row] |= (0x02 << sh);
		if (v % 5 > 2) _matrix_red[row] |= (0x04 << sh);
		if (v % 5 > 3) _matrix_red[row] |= (0x08 << sh);
	
		// draw the lines
		if (v > 4) _matrix_grn[row+1] |= (0x0F << sh); 
		if (v > 9) _matrix_grn[row+2] |= (0x0F << sh); 
		if (v > 14) _matrix_grn[row+3] |= (0x0F << sh); 
	}
	
	if (_segments[0] == 0) {
		_segments[0] = ' ';
		
		if (_segments[1] == 0) {
			_segments[1] = ' ';
			
			if (_segments[2] == 0) {
				_segments[2] = ' ';
			}
		}
	}
}

/*
 * This method was first proposed in the 1850s by John W. Nystrom
 */
void clock_hexadecimal(uint32_t ms) {
	//milliseconds to hexadecimal (F_FF_F)
	uint8_t hr = ms / 5400000;		// 1/16 day (hex hour) = 1 h 30 m
	ms -= 5400000 * (uint32_t) hr;
	uint8_t mx = ms / 337500;		// 1/256 day (hex maxime) = 5 m 37.5 s
	ms -= 337500 * (uint32_t) mx;
	ms *= 100;						// bump up the precision
	uint8_t mn = ms / 2109375;		// 1/4096 day (hex minute) ~= 21 seconds
	ms -= 2109375 * (uint32_t) mn;
	ms *= 100;						// bump up the precision again
	uint8_t sc = ms / 13183593;		// 1/65536 day (hex second) ~= 1.32 seconds
	
	_segments[0] = hr;
	_segments[1] = mx;
	_segments[2] = mn;
	_segments[3] = sc;
	
	_segment_flags = 0;
	
	clock_clear_matrix();

	// 2x2 pixels for each bit, hr on top, sc on bottom
	for (uint8_t i = 0; i < 4; i++) { // segments (rows)
		// build a 1x8 bar
		int v = _segments[i];
		for (uint8_t j = 0; j < 4; j++) { // bits (cols)
			if ((v & _BV(j)) != 0) {
				if (i == 1 || i == 3) {
					_matrix_grn[(i*2)+0] |= 1 << (7-(j*2));
					_matrix_grn[(i*2)+1] |= 1 << (7-(j*2));
					_matrix_grn[(i*2)+0] |= 1 << (6-(j*2));
					_matrix_grn[(i*2)+1] |= 1 << (6-(j*2));
				} else {
					_matrix_red[(i*2)+0] |= 1 << (7-(j*2));
					_matrix_red[(i*2)+1] |= 1 << (7-(j*2));
					_matrix_red[(i*2)+0] |= 1 << (6-(j*2));
					_matrix_red[(i*2)+1] |= 1 << (6-(j*2));
				}
			}
		}
	}
}

/*
 * This method was introduced during the French Revolution in 1793
 */
void clock_decimal(uint32_t ms) {
	//milliseconds to decimal (999.99)
	uint8_t dd = ms / 8640000;	// 1/10 day (deciday) = 2 h 24 m
	ms -= 8640000 * (uint32_t) dd;
	uint8_t cd = ms / 864000;	// 1/100 day (centiday) = 14 m 24 s
	ms -= 864000 * (uint32_t) cd;
	uint8_t md = ms / 86400;	// 1/1000 day (milliday; beat) = 1 m 26.4 s
	ms -= 86400 * (uint32_t) md;
	uint8_t ud = ms / 8640;		// 1/10000 day = 8.64 s
	ms -= 8640 * (uint32_t) ud;
	uint8_t nd = ms / 864;		// 1/100000 day (centibeat) / .864 s
	
	_segments[0] = dd;
	_segments[1] = cd;
	_segments[2] = md;
	_segments[3] = ud;
	_segments[4] = nd;
	
	_segment_flags = 0;
	if ((nd & _BV(0)) == _BV(0)) {
		_segment_flags |= _BV(4);
	}
	
	clock_clear_matrix();

	for (uint8_t i = 0; i < 4; i++) { // segments (rows)
		uint8_t v = _segments[i];
		int row = 2;
		if (i > 1) row += 5; // move md and ud down five rows
		// draw the dots
		if (i == 1 || i == 2) { // make cd and md green instead of red
			uint8_t sh = (i == 1) ? 5 : 0; // shift cd over three cols
			if (v == 1) {
				_matrix_grn[row-1] = (0x02 << sh);
			} else if (v == 2) {
				_matrix_grn[row-0] = (0x01 << sh);
				_matrix_grn[row-2] = (0x04 << sh);
			} else if (v == 3) {
				_matrix_grn[row-0] = (0x01 << sh);
				_matrix_grn[row-1] = (0x02 << sh);
				_matrix_grn[row-2] = (0x04 << sh);
			} else if (v == 4) {
				_matrix_grn[row-0] = (0x05 << sh);
				_matrix_grn[row-2] = (0x05 << sh);
			} else if (v == 5) {
				_matrix_grn[row-0] = (0x05 << sh);
				_matrix_grn[row-1] = (0x02 << sh);
				_matrix_grn[row-2] = (0x05 << sh);
			} else if (v == 6) {
				_matrix_grn[row-0] = (0x05 << sh);
				_matrix_grn[row-1] = (0x05 << sh);
				_matrix_grn[row-2] = (0x05 << sh);
			} else if (v == 7) {
				_matrix_grn[row-0] = (0x05 << sh);
				_matrix_grn[row-1] = (0x07 << sh);
				_matrix_grn[row-2] = (0x05 << sh);
			} else if (v == 8) {
				_matrix_grn[row-0] = (0x07 << sh);
				_matrix_grn[row-1] = (0x05 << sh);
				_matrix_grn[row-2] = (0x07 << sh);
			} else if (v == 9) {
				_matrix_grn[row-0] = (0x07 << sh);
				_matrix_grn[row-1] = (0x07 << sh);
				_matrix_grn[row-2] = (0x07 << sh);
			}
		} else {
			uint8_t sh = (i == 3) ? 5 : 0; // shift ud over three cols
			if (v == 1) {
				_matrix_red[row-1] = (0x02 << sh);
			} else if (v == 2) {
				_matrix_red[row-0] = (0x01 << sh);
				_matrix_red[row-2] = (0x04 << sh);
			} else if (v == 3) {
				_matrix_red[row-0] = (0x01 << sh);
				_matrix_red[row-1] = (0x02 << sh);
				_matrix_red[row-2] = (0x04 << sh);
			} else if (v == 4) {
				_matrix_red[row-0] = (0x05 << sh);
				_matrix_red[row-2] = (0x05 << sh);
			} else if (v == 5) {
				_matrix_red[row-0] = (0x05 << sh);
				_matrix_red[row-1] = (0x02 << sh);
				_matrix_red[row-2] = (0x05 << sh);
			} else if (v == 6) {
				_matrix_red[row-0] = (0x05 << sh);
				_matrix_red[row-1] = (0x05 << sh);
				_matrix_red[row-2] = (0x05 << sh);
			} else if (v == 7) {
				_matrix_red[row-0] = (0x05 << sh);
				_matrix_red[row-1] = (0x07 << sh);
				_matrix_red[row-2] = (0x05 << sh);
			} else if (v == 8) {
				_matrix_red[row-0] = (0x07 << sh);
				_matrix_red[row-1] = (0x05 << sh);
				_matrix_red[row-2] = (0x07 << sh);
			} else if (v == 9) {
				_matrix_red[row-0] = (0x07 << sh);
				_matrix_red[row-1] = (0x07 << sh);
				_matrix_red[row-2] = (0x07 << sh);
			}
		}
	}

	if (nd == 1 || nd == 4 || nd == 7 || nd == 3 || nd == 6 || nd == 9) {
		_matrix_red[3] = 0x18;
		_matrix_red[4] = 0x18;
	}
	if (nd == 2 || nd == 5 || nd == 8 || nd == 3 || nd == 6 || nd == 9) {
		_matrix_grn[3] = 0x18;
		_matrix_grn[4] = 0x18;
	}
	
	if (_segments[0] == 0) {
		_segments[0] = ' ';
		if (_segments[1] == 0) {
			_segments[1] = ' ';
		}
	}
}

void clock_update(uint32_t ms) {
	switch (_mode) {
		case 0: clock_traditional(ms); break;
		case 1: clock_vigesimal(ms); break;
		case 2: clock_hexadecimal(ms); break;
		case 3: clock_decimal(ms); break;
	}
}

uint32_t clock_size_b() {
	switch (_mode) {
		case 0: return 3600000;
		case 1: return 216000;
		case 2: return 337500;
		case 3: return 864000;
	}
	return 0;
}

uint32_t clock_size_d() {
	switch(_mode) {
		case 0: return 60000;
		case 1: return 540;
		case 2: return 1318;
		case 3: return 8640;
	}
	return 0;
}
