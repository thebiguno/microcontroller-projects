#include "clock.h"
#include "../lib/draw/draw.h"
#include "../lib/draw/fonts/f_3x3.h"
#include "../lib/draw/fonts/cp_ascii.h"

#include <avr/sfr_defs.h>

/*
 * This is the normal method of keeping time used worldwide
 */
void clock_traditional(struct time_t *t) {
	char year[5];
	char date[5];
	char time[5];
	
	uint16_t x = t->year;
	year[0] = (uint8_t) (x / 1000 + 48);
	x -= x / 1000 * 1000;
	year[1] = (uint8_t) (x / 100 + 48);
	x -= x / 100 * 100;
	year[2] = (uint8_t) (x / 10 + 48);
	x -= x / 10 * 10;
	year[3] = (uint8_t) (x + 48);
	
	year[4] = 0;

	uint8_t y = t->month;
	date[0] = y / 10 + 48;
	y -= y / 10 * 10;
	date[1] = y + 48;
	
	y = t->day;
	date[2] = y / 10 + 48;
	y -= y / 10 * 10;
	date[3] = y + 48;
	
	date[4] = 0;
	
	y = t->hour;
	time[0] = y / 10 + 48;
	y -= y / 10 * 10;
	time[1] = y + 48;
	
	y = t->minute;
	time[2] = y / 10 + 48;
	y -= y / 10 * 10;
	time[3] = y + 48;
	
	time[4] = 0;

	draw_rectangle(0,0,16,16,1,0x00,OVERLAY_REPLACE);
	draw_text(0, 0, year, 3, 3, ORIENTATION_NORMAL, font_3X3, codepage_ascii, RED_3, OVERLAY_REPLACE);
	draw_text(0, 4, date, 3, 3, ORIENTATION_NORMAL, font_3X3, codepage_ascii, RED_3, OVERLAY_REPLACE);
	draw_text(0, 8, time, 3, 3, ORIENTATION_NORMAL, font_3X3, codepage_ascii, RED_3, OVERLAY_REPLACE);
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
	uint8_t col = 23;
	for (uint8_t i = 0; i < 4; i++) { // digits
		uint8_t v = digits[i];
		
		uint8_t row = i * 4;
		uint8_t group = i % 2 == 0 ? GRN_3 | RED_3 : RED_3;
		
		color = (v & 0x01) ? group: 0x00;
		set_pixel(col, row + 3, color, OVERLAY_REPLACE); 

		color = (v & 0x02) ? group : 0x00;
		set_pixel(col, row + 2, color, OVERLAY_REPLACE); 

		color = (v & 0x04) ? group : 0x00;
		set_pixel(col, row + 1, color, OVERLAY_REPLACE); 

		color = (v & 0x08) ? group : 0x00;
		set_pixel(col, row + 0, color, OVERLAY_REPLACE); 
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
	uint8_t col = 22;
	for (uint8_t i = 0; i < 5; i++) { // digits
		uint8_t v = digits[i];
		
		uint8_t row = i * 3 + 1;
		uint8_t group = i % 2 == 0 ? GRN_3 | RED_3 : RED_3;
		
		color = (v & 0x01) ? group : 0x00;
		set_pixel(col, row + 2, color, OVERLAY_REPLACE); 

		color = (v & 0x02) ? group : 0x00;
		set_pixel(col, row + 1, color, OVERLAY_REPLACE); 

		color = (v & 0x04) ? group : 0x00;
		set_pixel(col, row + 0, color, OVERLAY_REPLACE); 
	}
}

/*
 * Sets a matrix array according to the time in the given mode.
 */
void clock_draw(struct time_t *t, uint32_t ms) {
//	draw_rectangle(0,0, 15,15, DRAW_FILLED, 0x00, OVERLAY_REPLACE);
	
	clock_traditional(t);
//	clock_vigesimal(ms);
	clock_hexadecimal(ms);
//	clock_dozenal(ms);
//	clock_decimal(ms);
	clock_octal(ms);
}


