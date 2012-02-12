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
	//milliseconds to traditional (24:59)
	uint8_t hr = ms / 3600000;	// 1/24 day (hour)
	ms -= hr * 3600000;
	uint8_t mn = ms / 60000;	// 1/1440 day (minute)
	ms -= mn / 60000;
	uint8_t sc = ms / 1000;		// 1/86400 day (second)	
	
	clock_clear_matrix();
	
	if (hr >= 20) {
		_segments[0] = 2;
		hr -= 20;
	} else if (hr >= 10) {
		_segments[0] = 1;
		hr -= 10;
	} else {
		_segments[0] = 0;
	}
	_segments[1] = hr;
	
	if (mn >= 50) {
		_segments[2] = 5;
		mn -= 50;
	} else if (mn >= 40) {
		_segments[2] = 4;
		mn -= 40;
	} else if (mn >= 30) {
		_segments[2] = 3;
		mn -= 30;
	} else if (mn >= 20) {
		_segments[2] = 2;
		mn -= 20;
	} else if (mn >= 10) {
		_segments[2] = 1;
		mn -= 10;
	} else {
		_segments[2] = 0;
	}
	_segments[3] = mn;
}

void clock_vigesimal(uint32_t ms) {
	//milliseconds to vigesimal (19:19:19:19)
	uint8_t a = ms / 4320000;	// 1/20 day
	ms -= a * 4320000;
	uint8_t b = ms / 216000;	// 1/400 day
	ms -= b * 216000;
	uint8_t c = ms / 10800;		// 1/8000 day
	ms -= c * 10800;
	uint8_t d = ms / 540;		// 1/160000 day
	
	_segments[0] = a;
	_segments[1] = b;
	_segments[2] = c;
	_segments[3] = d;

	clock_clear_matrix();

	for (uint8_t i = 0; i < 4; i ++) {
		// build a 4x4 mayan number square
		uint8_t sq[4] = {0,0,0,0};
		uint8_t v = _segments[i];
		uint8_t m = v % 5;
		if (m > 3) sq[3] = 8;
		if (m > 2) sq[2] = 8;
		if (m > 1) sq[1] = 8;
		if (m > 0) sq[0] = 8;
		for (uint8_t j = 0; j < 4; j++) {
			if (v > 14) {
				sq[j] = sq[j] >> 3;
				sq[j] |= 7;
			} else if (v > 9) {
				sq[j] = sq[j] >> 2;
				sq[j] |= 3;
			} else if (v > 4) {
				sq[j] = sq[j] >> 1;
				sq[j] |= 1;
			}
			if (i == 0 || i == 2) {
				sq[j] = sq[j] >> 4;
			}
		}
		
		// copy the 4x4 square into the matrix
		uint8_t offset = 0;
		if (i == 1 || i == 3) offset = 4;
		for (int i = 0; i < 4; i++) {
			_matrix_red[i + offset] |= sq[i];
		}
	}
}

void clock_hexadecimal(uint32_t ms) {
	double msd = ms;
	//milliseconds to hexadecimal (F:F:F:F)
	uint8_t hr = msd / 5400000;		// 1/16 day (hex hour)
	msd -= 5400000 * hr;
	uint8_t mx = msd / 337500;		// 1/256 day (hex maxime)
	msd -= 337500 * mx;
	uint8_t mn = msd / 21093.75;		// 1/4096 day (hex minute)
	msd -= 21093.75 * mn;
	uint8_t sc = msd / 1318.359375;	// 1/65536 day (hex second)
	
	_segments[0] = hr;
	_segments[1] = mx;
	_segments[2] = mn;
	_segments[3] = sc;
	
	clock_clear_matrix();

	// 2x2 pixels for each bit, hr on top, sc on bottom
	for (uint8_t i = 0; i < 4; i++) {
		// build a 1x8 bar
		int v = _segments[i];
		for (uint8_t j = 0; j < 4; j++) {
			if ((v & _BV(j)) != 0) {
				_matrix_red[(i*2)+0] |= 1 << (7-(j*2));
				_matrix_red[(i*2)+1] |= 1 << (7-(j*2));
				_matrix_red[(i*2)+0] |= 1 << (6-(j*2));
				_matrix_red[(i*2)+1] |= 1 << (6-(j*2));
			}
		}
	}
}

void clock_decimal(uint32_t ms) {
	//milliseconds to decimal (9:9:9:9)
	int dd = ms / 8640000;	// 1/10 day (deciday)
	ms -= dd * 8640000;
	int cd = ms / 864000;	// 1/100 day (centiday)
	ms -= cd * 864000;
	int md = ms / 86400;	// 1/1000 day (milliday)
	ms -= md * 86400;
	int ud = ms / 8640;		// 1/10000 day (microday)
	
	_segments[0] = dd;
	_segments[1] = cd;
	_segments[2] = md;
	_segments[3] = ud;
	
	clock_clear_matrix();
}

void clock_octal(uint32_t ms) {
	double msd = ms;
	//milliseconds to octal (7:7:7:7:7:7)
	uint8_t a = ms / 10800000;	// 1/8 day
	msd -= a * 10800000;
	uint8_t b = msd / 1350000;	// 1/64 day
	msd -= b * 1350000;
	uint8_t c = msd / 168750;	// 1/512 day
	msd -= c * 168750;
	uint8_t d = msd / 21093.75;	// 1/4096 day
	msd -= d * 21093.75;
	uint8_t e = msd / 2636.71875;	// 1/32768 day
	msd -= e * 2636.71875;
	uint8_t f = msd / 329.58984375;	// 1/262144 day
	
	_segments[0] = a;
	_segments[1] = b;
	_segments[2] = c;
	_segments[3] = d;
	_segments[4] = e;
	_segments[5] = f;
	
	clock_clear_matrix();
	
	// 1x2 pixels for each bit, a on top, f on bottom
	for (int i = 0; i < 6; i++) {
		// build a 1x6 bar
		int v = _segments[i];
		for (int j = 0; j < 3; j++) {
			if ((v & _BV(j)) != 0) {
				_matrix_red[6-(j*2)] |= (2 << i);
				_matrix_red[5-(j*2)] |= (2 << i);
			}
		}
	}
}

void clock_dni(uint32_t ms) {
	//milliseconds to d'ni (25:25:25:25)
	int a = ms / 3456000;	// 1/25 day
	ms -= a * 3456000;
	int b = ms / 138240;	// 1/625 day
	ms -= b * 138240;
	int c = ms / 5530;		// 1/15625 day
	ms -= c * 5530;
	int d = ms / 221;		// 1/390625 day
}

void clock_update(uint32_t ms) {
	switch (_mode) {
		case 0: clock_traditional(ms); break;
		case 1: clock_vigesimal(ms); break;
		case 2: clock_hexadecimal(ms); break;
		case 3: clock_decimal(ms); break;
		case 4: clock_octal(ms); break;
	}
}