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

	uint8_t color = 0x00;
	for (uint8_t i = 0; i < 6; i++) { // digits
		uint8_t v = digits[i];
		uint8_t col = OFFSET_TRAD_X + i;
		if (i > 1) col++; // blank columns
		if (i > 3) col++;
		
		color = (v & 0x01) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_TRAD_Y + 7, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_TRAD_Y + 6, color, OVERLAY_REPLACE);

		color = (v & 0x02) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_TRAD_Y + 5, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_TRAD_Y + 4, color, OVERLAY_REPLACE);

		color = (v & 0x04) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_TRAD_Y + 3, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_TRAD_Y + 2, color, OVERLAY_REPLACE);

		color = (v & 0x08) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_TRAD_Y + 1, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_TRAD_Y + 0, color, OVERLAY_REPLACE);
	}
}

uint8_t fade_g_to_r(uint8_t curr) {
	switch (curr) {
		case GRN_3: return GRN_2 | RED_1;
		case GRN_2 | RED_1: return GRN_1 | RED_2;
		case GRN_1 | RED_2: return RED_3;
		default: return GRN_3;
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
	
		uint8_t col = OFFSET_VIG_X;
		uint8_t row_offset = OFFSET_VIG_Y + (i > 1) ? 4 : 0; // move blocks c and d down four rows
		uint8_t row = row_offset + 3;
		uint8_t v = digits[i];
		if (i % 2 == 1) col = col + 4; // move blocks b and d over 4 cols
	
		uint8_t color = GRN_2 | RED_1;
		// draw the lines
		if (v > 14) {
			row--;
			draw_line(col, row_offset + 1, col + 3, row_offset + 1, color, OVERLAY_REPLACE);
			color = fade_g_to_r(color);
		}
		if (v > 9) {
			row--;
			draw_line(col, row_offset + 2, col + 3, row_offset + 2, color, OVERLAY_REPLACE);
			color = fade_g_to_r(color);
		}
		if (v > 4) {
			row--;
			draw_line(col, row_offset + 3, col + 3, row_offset + 3, color, OVERLAY_REPLACE);
			color = fade_g_to_r(color);
		}
		
		// draw the dots
		color = GRN_3;
		if (v % 5 > 3) {
			set_pixel(col + 0, row, color, OVERLAY_REPLACE);
			color = fade_g_to_r(color);
		}
		if (v % 5 > 2) {
			set_pixel(col + 1, row, color, OVERLAY_REPLACE);
			color = fade_g_to_r(color);
		}
		if (v % 5 > 1) {
			set_pixel(col + 2, row, color, OVERLAY_REPLACE);
			color = fade_g_to_r(color);
		}
		if (v % 5 > 0) {
			set_pixel(col + 3, row, color, OVERLAY_REPLACE);
			color = fade_g_to_r(color);
		}
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
	
	uint8_t color = 0x00;
	
	for (uint8_t i = 0; i < 4; i++) { // digits
		uint8_t col = OFFSET_HEX_X + (i * 2);
		uint8_t v = digits[i];

		color = (v & 0x01) ? GRN_3 | RED_3: 0x00;
		set_pixel(col, OFFSET_HEX_Y + 7, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_HEX_Y + 6, color, OVERLAY_REPLACE);
		set_pixel(col + 1, OFFSET_HEX_Y + 7, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col + 1, OFFSET_HEX_Y + 6, color & RED_1, OVERLAY_REPLACE);

		color = (v & 0x02) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_HEX_Y + 5, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_HEX_Y + 4, color, OVERLAY_REPLACE);
		set_pixel(col + 1, OFFSET_HEX_Y + 5, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col + 1, OFFSET_HEX_Y + 4, color & RED_1, OVERLAY_REPLACE);

		color = (v & 0x04) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_HEX_Y + 3, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_HEX_Y + 2, color, OVERLAY_REPLACE);
		set_pixel(col + 1, OFFSET_HEX_Y + 3, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col + 1, OFFSET_HEX_Y + 2, color & RED_1, OVERLAY_REPLACE);

		color = (v & 0x08) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_HEX_Y + 1, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_HEX_Y + 0, color, OVERLAY_REPLACE);
		set_pixel(col + 1, OFFSET_HEX_Y + 1, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col + 1, OFFSET_HEX_Y + 0, color & RED_1, OVERLAY_REPLACE);
	}
}

void clock_dozenal(uint32_t ms) {
	uint8_t digits[5];
	//milliseconds to decimal (BBB.BB)
	digits[0] = ms / 7200000;	// 1/12 day = 2 h
	ms -= 7200000 * (uint32_t) digits[0];
	digits[1] = ms / 600000;	// 1/144 day = 10 m
	ms -= 600000 * (uint32_t) digits[1];
	digits[2] = ms / 50000;		// 1/1728 day = 50 s
	ms -= 50000 * (uint32_t) digits[2];
	digits[3] = ms / 4167;		// 1/20736 day ~= 4.167 s
	ms -= 4167 * (uint32_t) digits[3];
	digits[4] = ms / 347;		// 1/248832 day ~= .347 s

	uint8_t color = 0x00;
	for (uint8_t i = 0; i < 5; i++) { // digits
		uint8_t col = OFFSET_DOZ_X + (i < 3 ? i * 2 : i + 3);
		uint8_t v = digits[i];

		color = (v & 0x01) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_DOZ_Y + 7, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DOZ_Y + 6, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DOZ_Y + 7, color & RED_1, OVERLAY_REPLACE);
			set_pixel(col + 1, OFFSET_DOZ_Y + 6, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x02) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_DOZ_Y + 5, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DOZ_Y + 4, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DOZ_Y + 5, color & RED_1, OVERLAY_REPLACE);
			set_pixel(col + 1, OFFSET_DOZ_Y + 4, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x04) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_DOZ_Y + 3, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DOZ_Y + 2, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DOZ_Y + 3, color & RED_1, OVERLAY_REPLACE);
			set_pixel(col + 1, OFFSET_DOZ_Y + 2, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x08) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_DOZ_Y + 1, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DOZ_Y + 0, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DOZ_Y + 1, color & RED_1, OVERLAY_REPLACE);
			set_pixel(col + 1, OFFSET_DOZ_Y + 0, color & RED_1, OVERLAY_REPLACE);
		}
	}
}

/*
 * This method was introduced during the French Revolution in 1793
 * It's also equivalent to Swatch time
 */
void clock_decimal(uint32_t ms) {
	uint8_t digits[5];
	//milliseconds to decimal (999.99)
	digits[0] = ms / 8640000;	// 1/10 day (deciday) = 2 h 24 m
	ms -= 8640000 * (uint32_t) digits[0];
	digits[1] = ms / 864000;	// 1/100 day (centiday) = 14 m 24 s
	ms -= 864000 * (uint32_t) digits[1];
	digits[2] = ms / 86400;		// 1/1000 day (milliday; beat) = 1 m 26.4 s
	ms -= 86400 * (uint32_t) digits[2];
	digits[3] = ms / 8640;		// 1/10000 day (decibeat)= 8.64 s
	ms -= 8640 * (uint32_t) digits[3];
	digits[4] = ms / 864;		// 1/100000 day (centibeat) / .864 s
	
	uint8_t color = 0x00;
	for (uint8_t i = 0; i < 5; i++) { // digits
		uint8_t col = OFFSET_DEC_X + (i < 3 ? i * 2 : i + 3);
		uint8_t v = digits[i];

		color = (v & 0x01) ? GRN_3 | RED_3: 0x00;
		set_pixel(col, OFFSET_DEC_Y + 7, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DEC_Y + 6, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DEC_Y + 7, color & RED_1, OVERLAY_REPLACE); 
			set_pixel(col + 1, OFFSET_DEC_Y + 6, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x02) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_DEC_Y + 5, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DEC_Y + 4, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DEC_Y + 5, color & RED_1, OVERLAY_REPLACE); 
			set_pixel(col + 1, OFFSET_DEC_Y + 4, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x04) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_DEC_Y + 3, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DEC_Y + 2, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DEC_Y + 3, color & RED_1, OVERLAY_REPLACE); 
			set_pixel(col + 1, OFFSET_DEC_Y + 2, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x08) ? GRN_3 | RED_3 : 0x00;
		set_pixel(col, OFFSET_DEC_Y + 1, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_DEC_Y + 0, color, OVERLAY_REPLACE);
		if (i < 3) {
			set_pixel(col + 1, OFFSET_DEC_Y + 1, color & RED_1, OVERLAY_REPLACE); 
			set_pixel(col + 1, OFFSET_DEC_Y + 0, color & RED_1, OVERLAY_REPLACE);
		}
	}

}

void clock_octal(uint32_t ms) {
	//milliseconds to octal (777.777)
	uint8_t digits[6];
	digits[0] = ms / 10800000;		// 1/8 day = 3 h
	ms -= 10800000 * (uint32_t) digits[0] ;
	digits[1] = ms / 1350000;		// 1/64 day = 22 m 30 s
	ms -= 1350000 * (uint32_t) digits[1];
	digits[2] = ms / 168750;		// 1/512 day ~= 2 m 49 s
	ms -= 168750 * (uint32_t) digits[2];
	ms *= 100;						// bump up the precision
	digits[3] = ms / 2109375;		// 1/4096 day ~= 21 s
	ms -= 2109375 * (uint32_t) digits[3];
	ms *= 100;						// bump up the precision again
	digits[4] = ms / 26367187;		// 1/32768 day ~= 2.63 s
	ms -= 26367187 * (uint32_t) digits[4];
	ms *= 100;						// bump up the precision again
	digits[5] = ms / 329589843;		// 1/262144 day ~= .329 s

	uint8_t color = 0x00;
	for (uint8_t i = 0; i < 6; i++) { // digits
		uint8_t col = OFFSET_OCT_X + (i < 2 ? i * 2 : i + 2);
		uint8_t v = digits[i];

		color = (v & 0x01) ? GRN_3 | RED_3: 0x00;
		set_pixel(col, OFFSET_OCT_Y + 7, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_OCT_Y + 6, color, OVERLAY_REPLACE);
		if (i < 2) {
			set_pixel(col + 1, OFFSET_OCT_Y + 7, color & RED_1, OVERLAY_REPLACE); 
			set_pixel(col + 1, OFFSET_OCT_Y + 6, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x02) ? GRN_3 | RED_3: 0x00;
		set_pixel(col, OFFSET_OCT_Y + 5, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_OCT_Y + 4, color, OVERLAY_REPLACE);
		if (i < 2) {
			set_pixel(col + 1, OFFSET_OCT_Y + 5, color & RED_1, OVERLAY_REPLACE); 
			set_pixel(col + 1, OFFSET_OCT_Y + 4, color & RED_1, OVERLAY_REPLACE);
		}
		
		color = (v & 0x04) ? GRN_3 | RED_3: 0x00;
		set_pixel(col, OFFSET_OCT_Y + 3, color & RED_1, OVERLAY_REPLACE); 
		set_pixel(col, OFFSET_OCT_Y + 2, color, OVERLAY_REPLACE);
		if (i < 2) {
			set_pixel(col + 1, OFFSET_OCT_Y + 3, color & RED_1, OVERLAY_REPLACE); 
			set_pixel(col + 1, OFFSET_OCT_Y + 2, color & RED_1, OVERLAY_REPLACE);
		}
		
		// color = (v & 0x08) ? GRN_3 : 0x00;
		// set_pixel(col, OFFSET_DEC_Y + 1, color, OVERLAY_REPLACE); 
		// set_pixel(col, OFFSET_DEC_Y + 0, color, OVERLAY_REPLACE);
		// set_pixel(col + 1, OFFSET_DEC_Y + 1, color, OVERLAY_REPLACE); 
		// set_pixel(col + 1, OFFSET_DEC_Y + 0, color, OVERLAY_REPLACE);
	}
}

/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_draw(uint32_t ms) {
	draw_rectangle(0,0, 15,15, DRAW_FILLED, 0x00, OVERLAY_REPLACE);
	
	clock_traditional(ms);
	clock_vigesimal(ms);
	clock_hexadecimal(ms);
	clock_dozenal(ms);
	clock_decimal(ms);
	clock_octal(ms);
}


