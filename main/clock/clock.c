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
			uint8_t offset = 0;
			if (i > 1) offset++;
			if (i > 3) offset++;
			if (i == 2 || i == 3) offset += 8; // overflow hack to draw into green matrix instead of red matrix
			if ((v & _BV(j)) != 0) {
				_matrix_red[i+offset] |= 1 << (7-(j*2));
				_matrix_red[i+offset] |= 1 << (6-(j*2));
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
		if (i > 1) row += 4; // move down 4 rows for c and d
		
		// draw the dots
		uint8_t sh = (i % 2 == 1) ? 4 : 0;
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
	double msd = ms;
	//milliseconds to hexadecimal (F:F:F:F)
	uint8_t hr = msd / 5400000;		// 1/16 day (hex hour)
	msd -= 5400000 * (double) hr;
	uint8_t mx = msd / 337500;		// 1/256 day (hex maxime)
	msd -= 337500 * (double) mx;
	uint8_t mn = msd / 21093.75;	// 1/4096 day (hex minute)
	msd -= 21093.75 * (double) mn;
	uint8_t sc = msd / 1318.359375;	// 1/65536 day (hex second)
	
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
			uint8_t offset = i % 2 == 0 ? 0 : 8; // overflow hack to draw into green matrix instead of red matrix
			if ((v & _BV(j)) != 0) {
				_matrix_red[(i*2)+0+offset] |= 1 << (7-(j*2));
				_matrix_red[(i*2)+1+offset] |= 1 << (7-(j*2));
				_matrix_red[(i*2)+0+offset] |= 1 << (6-(j*2));
				_matrix_red[(i*2)+1+offset] |= 1 << (6-(j*2));
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
	ms -= 86400 * (uint32_t) ud;
	int nd = ms / 864;		// 1/100000 day (nanoday??)
	
	_segments[0] = dd;
	_segments[1] = cd;
	_segments[2] = md;
	_segments[3] = ud;
	_segments[4] = nd;
	
	clock_clear_matrix();
	
	//
}

void clock_octal(uint32_t ms) {
	double msd = ms;
	//milliseconds to octal (7:7:7:7:7:7)
	uint8_t a = ms / 10800000;	// 1/8 day
	msd -= 10800000 * (double) a ;
	uint8_t b = msd / 1350000;	// 1/64 day
	msd -= 1350000 * (double) b;
	uint8_t c = msd / 168750;	// 1/512 day
	msd -= 168750 * (double) c;
	uint8_t d = msd / 21093.75;	// 1/4096 day
	msd -= 21093.75 * (double) d;
	uint8_t e = msd / 2636.71875;	// 1/32768 day
	msd -= 2636.71875 * (double) e;
	uint8_t f = msd / 329.58984375;	// 1/262144 day
	
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
			uint8_t offset = i % 2 == 0 ? 0 : 8; // overflow hack to draw into green matrix instead of red matrix
			if ((v & _BV(j)) != 0) {
				_matrix_red[i+1+offset] |= 2 << (7-(j*2));
				_matrix_red[i+1+offset] |= 2 << (6-(j*2));
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