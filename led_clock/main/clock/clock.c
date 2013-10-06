#include "clock.h"
#include "../lib/draw/draw.h"
#include "../lib/draw/fonts/f_3x5.h"
#include "../lib/draw/fonts/cp_ascii.h"
#include <avr/sfr_defs.h>

/*
 * This is the normal method of keeping time used worldwide
 */
void clock_traditional(struct time_t *t) {
	
	draw_rectangle(0,0,23,15, 1, 0x00, OVERLAY_REPLACE);

	set_pixel(7, 0, GRN_1, OVERLAY_REPLACE);
	set_pixel(11, 0, GRN_1, OVERLAY_REPLACE);
	set_pixel(14, 3, GRN_1, OVERLAY_REPLACE);
	set_pixel(14, 7, GRN_1, OVERLAY_REPLACE);
	set_pixel(14, 11, GRN_1, OVERLAY_REPLACE);
	set_pixel(11, 14, GRN_1, OVERLAY_REPLACE);
	set_pixel(7, 14, GRN_1, OVERLAY_REPLACE);
	set_pixel(3, 14, GRN_1, OVERLAY_REPLACE);
	set_pixel(0, 11, GRN_1, OVERLAY_REPLACE);
	set_pixel(0, 7, GRN_1, OVERLAY_REPLACE);
	set_pixel(0, 3, GRN_1, OVERLAY_REPLACE);
	set_pixel(3, 0, GRN_1, OVERLAY_REPLACE);

	uint8_t c = RED_3 | GRN_3;
	uint8_t m = t->minute / 5;
	switch (m) {
		case 0: draw_line(7, 7, 7, 0, c, OVERLAY_REPLACE); break; // OK
		case 1: draw_line(7, 7, 11, 0, c, OVERLAY_REPLACE); break; // OK
		case 2: draw_line(7, 7, 14, 4, c, OVERLAY_REPLACE); break; // OK
		case 3: draw_line(7, 7, 14, 7, c, OVERLAY_REPLACE); break; // OK
		case 4: draw_line(7, 7, 14, 11, c, OVERLAY_REPLACE); break; // OK
		case 5: draw_line(7, 7, 11, 14, c, OVERLAY_REPLACE); break; // OK
		case 6: draw_line(7, 7, 7, 14, c, OVERLAY_REPLACE); break; // OK
		case 7: draw_line(7, 7, 3, 14, c, OVERLAY_REPLACE); break; // OK
		case 8: draw_line(7, 7, 0, 11, c, OVERLAY_REPLACE); break; // OK
		case 9: draw_line(7, 7, 0, 7, c, OVERLAY_REPLACE); break; // OK
		case 10: draw_line(7, 7, 0, 4, c, OVERLAY_REPLACE); break; // OK
		case 11: draw_line(7, 7, 3, 0, c, OVERLAY_REPLACE); break; // OK
	}
	
	c = RED_3;
	uint8_t h = t->hour % 12;
	switch (h) {
		case 0: draw_line(7, 7, 7, 4, c, OVERLAY_REPLACE); break; // 0-4
		case 1: draw_line(7, 7, 9, 4, c, OVERLAY_REPLACE); break; // 5-9
		case 2: draw_line(7, 7, 10, 6, c, OVERLAY_REPLACE); break; // 10-14
		case 3: draw_line(7, 7, 10, 7, c, OVERLAY_REPLACE); break; // 15-19
		case 4: draw_line(7, 7, 10, 9, c, OVERLAY_REPLACE); break; // 20-24
		case 5: draw_line(7, 7, 9, 10, c, OVERLAY_REPLACE); break; // 25-29
		case 6: draw_line(7, 7, 7, 10, c, OVERLAY_REPLACE); break; // 30-34
		case 7: draw_line(7, 7, 5, 10, c, OVERLAY_REPLACE); break; // 35-39
		case 8: draw_line(7, 7, 4, 9, c, OVERLAY_REPLACE); break; // 40-44
		case 9: draw_line(7, 7, 4, 7, c, OVERLAY_REPLACE); break; // 45-49
		case 10: draw_line(7, 7, 4, 6, c, OVERLAY_REPLACE); break; // 50-54
		case 11: draw_line(7, 7, 5, 4, c, OVERLAY_REPLACE); break; // 55-59
	}
	
	uint8_t s = t->second / 5;
	switch (s) {
		case 0: set_pixel(7, 0, c, OVERLAY_REPLACE); break;
		case 1: set_pixel(11, 0, c, OVERLAY_REPLACE); break;
		case 2: set_pixel(14, 3, c, OVERLAY_REPLACE); break;
		case 3: set_pixel(14, 7, c, OVERLAY_REPLACE); break;
		case 4: set_pixel(14, 11, c, OVERLAY_REPLACE); break;
		case 5: set_pixel(11, 14, c, OVERLAY_REPLACE); break;
		case 6: set_pixel(7, 14, c, OVERLAY_REPLACE); break;
		case 7: set_pixel(3, 14, c, OVERLAY_REPLACE); break;
		case 8: set_pixel(0, 11, c, OVERLAY_REPLACE); break;
		case 9: set_pixel(0, 7, c, OVERLAY_REPLACE); break;
		case 10: set_pixel(0, 3, c, OVERLAY_REPLACE); break;
		case 11: set_pixel(3, 0, c, OVERLAY_REPLACE); break;
	}
	
	switch(t->wday) {
		case 0: draw_text(16,0,(char*)"SU",3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE); break;
		case 1: draw_text(16,0,(char*)"MO",3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE); break;
		case 2: draw_text(16,0,(char*)"TU",3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE); break;
		case 3: draw_text(16,0,(char*)"WE",3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE); break;
		case 4: draw_text(16,0,(char*)"TH",3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE); break;
		case 5: draw_text(16,0,(char*)"FR",3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE); break;
		case 6: draw_text(16,0,(char*)"SA",3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE); break;
	}
	
	uint8_t x = t->day;
	char d[3];
	d[0] = x / 10 + 48;
	x -= x / 10 * 10;
	d[1] = x + 48;
	d[2] = 0;
	draw_text(16,6,d,3,5,ORIENTATION_NORMAL,font_3x5,codepage_ascii, RED_3, OVERLAY_REPLACE);
	
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
	uint8_t row = 15;
	for (uint8_t i = 0; i < 4; i++) { // digits
		uint8_t v = digits[i];
		
		uint8_t col = i * 4;
		uint8_t group = i % 2 == 0 ? GRN_3 | RED_3 : RED_3;
		
		color = (v & 0x01) ? group: 0x00;
		set_pixel(col + 3, row, color, OVERLAY_REPLACE); 

		color = (v & 0x02) ? group : 0x00;
		set_pixel(col + 2, row, color, OVERLAY_REPLACE); 

		color = (v & 0x04) ? group : 0x00;
		set_pixel(col + 1, row, color, OVERLAY_REPLACE); 

		color = (v & 0x08) ? group : 0x00;
		set_pixel(col + 0, row, color, OVERLAY_REPLACE); 
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
		
		color = (v & 0x02) ? GRN_3 | RED_3: 0x00;
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



/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_draw(struct time_t *t, uint32_t ms) {
//	draw_rectangle(0,0, 15,15, DRAW_FILLED, 0x00, OVERLAY_REPLACE);
	
	clock_traditional(t);
	clock_hexadecimal(ms);
//	clock_dozenal(ms);
//	clock_decimal(ms);
}


