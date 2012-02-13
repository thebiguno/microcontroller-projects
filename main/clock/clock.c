#include "clock.h"

#include <avr/sfr_defs.h>

static uint8_t _mode = 0;
static char _segments[6];
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

void clock_traditional(uint32_t ms) {
	//milliseconds to traditional (24:59:59)
	uint8_t hr = ms / 3600000;	// 1/24 day (hour)
	ms -= 3600000 * (uint32_t) hr;
	uint8_t mn = ms / 60000;	// 1/1440 day (minute)
	ms -= 60000 * (uint32_t) mn;
	uint8_t sc = ms / 1000;		// 1/86400 day (second)	
	
	_segments[0] = 0;
	for (uint8_t i = 1; i < 3; i++) {
		if (hr >= 10) {
			_segments[0] = i;
			hr -= 10;
		}
	}
	_segments[1] = hr;
	
	_segments[2] = 0;
	for (uint8_t i = 1; i < 6; i++) {
		if (mn >= 10) {
			_segments[2] = i;
			mn -= 10;
		}
	}
	_segments[3] = mn;
	
	_segments[4] = 0;
	for (uint8_t i = 1; i < 6; i++) {
		if (sc >= 10) {
			_segments[4] = i;
			sc -= 10;
		}
	}
	_segments[5] = sc;

	clock_clear_matrix();
	
	// 3 2x8 bars with a space sparating them, hr on top, sec on bottom
	
	for (int i = 0; i < 6; i++) { // segments (rows)
		// build a 1x6 bar
		int v = _segments[i];
		for (int j = 0; j < 8; j++) { // bits (cols)
			uint8_t row = i;
			if (i > 1) row++;
			if (i > 3) row++;
			if (i == 2 || i == 3) row += 8; // overflow hack to draw into green matrix instead of red matrix
			if ((v & _BV(j)) != 0) {
				_matrix_red[row] |= 1 << (7-(j*2));
				_matrix_red[row] |= 1 << (6-(j*2));
			}
		}
	}
}

void clock_vigesimal(uint32_t ms) {
	//milliseconds to vigesimal (19:19:19:19)
	uint8_t a = ms / 4320000;	// 1/20 day
	ms -= 4320000 * (uint32_t) a;
	uint8_t b = ms / 216000;	// 1/400 day
	ms -= 216000 * (uint32_t) b;
	uint8_t c = ms / 10800;		// 1/8000 day
	ms -= 10800 * (uint32_t) c;
	uint8_t d = ms / 540;		// 1/160000 day
	
	_segments[0] = a;
	_segments[1] = b;
	_segments[2] = c;
	_segments[3] = d;

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
}

void clock_hexadecimal(uint32_t ms) {
	//milliseconds to hexadecimal (F:F:F:F)
	uint8_t hr = ms / 5400000;		// 1/16 day (hex hour)
	ms -= 5400000 * (uint32_t) hr;
	uint8_t mx = ms / 337500;		// 1/256 day (hex maxime)
	ms -= 337500 * (uint32_t) mx;
	ms *= 100;						// bump up the precision
	uint8_t mn = ms / 2109375;		// 1/4096 day (hex minute)
	ms -= 2109375 * (uint32_t) mn;
	ms *= 100;						// bump up the precision again
	uint8_t sc = ms / 13183593;		// 1/65536 day (hex second)
	
	_segments[0] = hr;
	_segments[1] = mx;
	_segments[2] = mn;
	_segments[3] = sc;
	
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

void clock_decimal(uint32_t ms) {
	//milliseconds to decimal (9:9:9:9:9)
	int dd = ms / 8640000;	// 1/10 day (deciday)
	ms -= 8640000 * (uint32_t) dd;
	int cd = ms / 864000;	// 1/100 day (centiday)
	ms -= 864000 * (uint32_t) cd;
	int md = ms / 86400;	// 1/1000 day (milliday)
	ms -= 86400 * (uint32_t) md;
	int ud = ms / 8640;		// 1/10000 day (microday??)
	ms -= 8640 * (uint32_t) ud;
	int nd = ms / 864;		// 1/100000 day (nanoday??)
	
	_segments[0] = dd;
	_segments[1] = cd;
	_segments[2] = md;
	_segments[3] = ud;
	_segments[4] = nd;
	
	clock_clear_matrix();
	
	for (uint8_t i = 0; i < 4; i++) { // segments (rows)
		// build a 3x3 number square
		
		uint8_t v = _segments[i];
		int row = 3;
		if (i > 1) row += 3; // move md and ud down three rows
		if (i == 1 || i == 2) row += 8; // make cd and md green instead of red
		
		// draw the dots
		uint8_t sh = (i % 2 == 1) ? 3 : 0; // shift cd and ud over three cols
		if (v > 0) _matrix_red[row-0] |= (0x08 << sh);
		if (v > 1) _matrix_red[row-0] |= (0x04 << sh);
		if (v > 2) _matrix_red[row-0] |= (0x02 << sh);
		if (v > 3) _matrix_red[row-1] |= (0x08 << sh);
		if (v > 4) _matrix_red[row-1] |= (0x04 << sh);
		if (v > 5) _matrix_red[row-1] |= (0x02 << sh);
		if (v > 6) _matrix_red[row-2] |= (0x08 << sh);
		if (v > 7) _matrix_red[row-2] |= (0x04 << sh);
		if (v > 8) _matrix_red[row-2] |= (0x02 << sh);
	}
	
	// draw the border
	if (nd > 0) { 
		_matrix_red[0] |= 0x06; _matrix_grn[0] |= 0x06;
	}
	if (nd > 1) { 
		_matrix_red[0] |= 0x60; _matrix_grn[0] |= 0x60;
	}
	if (nd > 2) {
		_matrix_red[1] |= 0x80; _matrix_grn[1] |= 0x80;
		_matrix_red[2] |= 0x80; _matrix_grn[2] |= 0x80;
	}
	if (nd > 3) {
		_matrix_red[4] |= 0x80; _matrix_grn[4] |= 0x80;
		_matrix_red[5] |= 0x80; _matrix_grn[5] |= 0x80;
	}
	if (nd > 4) { 
		_matrix_red[7] |= 0xC0; _matrix_grn[7] |= 0xC0;
	}
	if (nd > 5) { 
		_matrix_red[7] |= 0x18; _matrix_grn[7] |= 0x18;
	}
	if (nd > 6) { 
		_matrix_red[7] |= 0x03; _matrix_grn[7] |= 0x03;
	}
	if (nd > 7) {
		_matrix_red[4] |= 0x01; _matrix_grn[4] |= 0x01;
		_matrix_red[5] |= 0x01; _matrix_grn[5] |= 0x01;
	}
	if (nd > 8) {
		_matrix_red[1] |= 0x01; _matrix_grn[1] |= 0x01;
		_matrix_red[2] |= 0x01; _matrix_grn[2] |= 0x01;
	}
}

void clock_octal(uint32_t ms) {
	//milliseconds to octal (7:7:7:7:7:7)
	uint8_t a = ms / 10800000;	// 1/8 day
	ms -= 10800000 * (uint32_t) a ;
	uint8_t b = ms / 1350000;	// 1/64 day
	ms -= 1350000 * (uint32_t) b;
	uint8_t c = ms / 168750;	// 1/512 day
	ms -= 168750 * (uint32_t) c;
	ms *= 100;					// bump up the precision
	uint8_t d = ms / 2109375;	// 1/4096 day
	ms -= 2109375 * (uint32_t) d;
	ms *= 100;					// bump up the precision again
	uint8_t e = ms / 26367187;	// 1/32768 day
	ms -= 26367187 * (uint32_t) e;
	ms *= 100;					// bump up the precision again
	uint8_t f = ms / 329589843;	// 1/262144 day
	
	_segments[0] = a;
	_segments[1] = b;
	_segments[2] = c;
	_segments[3] = d;
	_segments[4] = e;
	_segments[5] = f;
	
	clock_clear_matrix();
	
	// 1x2 pixels for each bit, a on top, f on bottom
	for (int i = 0; i < 6; i++) { // segments (rows)
		// build a 1x6 bar
		int v = _segments[i];
		for (int j = 0; j < 3; j++) { // bits (cols)
			if ((v & _BV(j)) != 0) {
				if (i == 1 || i == 3 || i == 5) {
					_matrix_grn[i+1] |= 2 << (5-(j*2));
					_matrix_grn[i+1] |= 2 << (4-(j*2));
				} else {
					_matrix_red[i+1] |= 2 << (5-(j*2));
					_matrix_red[i+1] |= 2 << (4-(j*2));
				}
			}
		}
	}
}

// void clock_dni(uint32_t ms) {
// 	//milliseconds to d'ni (25:25:25:25)
// 	int a = ms / 3456000;	// 1/25 day
// 	ms -= a * 3456000;
// 	int b = ms / 138240;	// 1/625 day
// 	ms -= b * 138240;
// 	int c = ms / 5530;		// 1/15625 day
// 	ms -= c * 5530;
// 	int d = ms / 221;		// 1/390625 day
// }

void clock_update(uint32_t ms) {
	switch (_mode) {
		case 0: clock_traditional(ms); break;
		case 1: clock_vigesimal(ms); break;
		case 2: clock_hexadecimal(ms); break;
		case 3: clock_decimal(ms); break;
		case 4: clock_octal(ms); break;
	}
}