uint8_t _mode = 0;
char _segments[4];
uint8_t _matrix_red[8];
uint8_t _matrix_grn[8];

void clock_mode(uint8_t mode) {
	_mode = mode;
}

/*
 * Sets a char array according to the time in the given mode.
 */
void clock_segments(char[] result) {
	for (int = 0; i < 4; i++) {
		result = _segments;
	}
}

/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_matrix(uint8_t[] red, uint8_t[] grn) {
	for (int = 0; i < 8; i++) {
		red[i] = _matrix_red[i];
		grn[i] = _matrix_grn[i];
	}
}


void clock_traditional(uint16_t ms) {
	//milliseconds to traditional (24:59)
	uint8_t hr = ms / 3600000;	// 1/24 day (hour)
	ms -= hr * 3600000;
	uint8_t mn = ms / 60000;	// 1/1440 day (minute)
	ms -= mn / 60000;
	uint8_t sc = ms / 1000;		// 1/86400 day (second)	
	
}

void clock_vigesimal(uint16_t ms) {
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

	_matrix = {0,0,0,0,0,0,0,0};
	uint8_t dot = { 0x80 };
	for (int i = 0; i < 4; i ++) {
		// build a 4x4 mayan number square
		uint8_t sq[4];
		uint8_t v = _segments[i];
		uint8_t m = v % 5;
		if (m == 1) sq = {8,0,0,0};
		else if (m == 2) sq = {8,8,0,0};
		else if (m == 3) sq = {8,8,8,0};
		else if (m == 4) sq = {8,8,8,8};
		for (j = 0; j < 4; j++) {
			if (v > 14) {
				sq[j] = sq[j] >> 3;
				sq[j] |= 7;
			} else if (v > 9) {
				sq[j] = sq[j] >> 2;
				sqj[] |= 3;
			} else if (v > 4) {
				sq[j] = sq[j] >> 1;
				sq[j] |= 1;
			}
			if (i == 0 || i == 2) {
				sq[j] = sq[j] >> 4
			}
		}
		
		// copy the 4x4 square into the matrix
		uint8_t offset = 0;
		if (i == 1 || i == 3) offset = 4;
		for (int i = 0; i < 4; i++) {
			_matrix[i + offset] |= sq[i];
		}
	}
}

void clock_hexadecimal(uint16_t ms) {
	//milliseconds to hexadecimal (F:F:F:F)
	int hr - ms / 5400000;	// 1/16 day (hex hour)
	ms -= hr * 5400000;
	int mx = ms / 337500;	// 1/256 day (hex maxime)
	ms -= mx * 337500;
	int mn = ms / 21094;	// 1/4096 day (hex minute)
	ms -= mn * 21094;
	int sc = ms / 1318;		// 1/65536 day (hex second)
	
	_segments[0] = hr;
	_segments[1] = mx;
	_segments[2] = mn;
	_segments[3] = sc;

	// 2x2 pixels for each bit
	uint8_t[] bars = { 0xC0, 0x30, 0x0C, 0x03 };
	for (int i = 0; i < 4; i++) {
		int v = _segments[i];
		_matrix[(i*2)+0] = 0x00;
		_matrix[(i*2)+1] = 0x00;
		for (int j = 0; j < 4; j++) {
			if (bit_is_set(i,j)) {
				_matrix[(i*2)+0] += bars[j];
				_matrix[(i*2)+1] += bars[j];
			}
		}
	}
}

void clock_decimal(uint16_t ms) {
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
}

void clock_octal(uint16_t ms) {
	//milliseconds to octal (8:8:8:8:8:8)
	int a = ms / 10800000;	// 1/8 day
	ms -= a * 10800000;
	int b = ms / 1350000;	// 1/64 day
	ms -= b * 1350000;
	int c = ms / 168750;	// 1/512 day
	ms -= c * 168750;
	int d = ms / 21094;		// 1/4096 day
	ms -= d * 21094;
	int e = ms / 2637;		// 1/32768 day
	ms -= e * 21094;
	int f = ms / 330;		// 1/262144 day
	
	_segments[0] = a;
	_segments[1] = b;
	_segments[2] = c;
	_segments[3] = d;
	
}

void clock_dni(uint16_t ms) {
	//milliseconds to d'ni (25:25:25:25)
	int a = ms / 3456000;	// 1/25 day
	ms -= a * 3456000;
	int b = ms / 138240;	// 1/625 day
	ms -= b * 138240;
	int c = ms / 5530;		// 1/15625 day
	ms -= c * 5530;
	int d = ms / 221;		// 1/390625 day	
}

void update(uint16_t ms) {
	switch (_mode) {
		case 0: clock_traditional(ms);
		case 1: clock_vigesimal(ms);
		case 2: clock_hexadecimal(ms);
		case 3: clock_decimal(ms);
		case 4: clock_octal(ms);
	}
}
	}
}