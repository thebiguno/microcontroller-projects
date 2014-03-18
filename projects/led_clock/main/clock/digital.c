#include "clock.h"
#include "../lib/draw/draw.h"
#include "../lib/draw/fonts/f_5x13.h"
#include "../lib/draw/fonts/cp_numbers.h"


void clock_draw(struct time_t *t) {
	draw_rectangle(0, 0, 23, 15, DRAW_FILLED, 0x00, OVERLAY_REPLACE);
	
	char temp[3];
	temp[2] = 0x00;
	temp[0] = (t->hour / 10) + 48;
	temp[1] = (t->hour % 10) + 48;
	draw_text(0, 0, temp, 5, 13, ORIENTATION_NORMAL, font_5x13, codepage_numbers, GRN_3, OVERLAY_REPLACE);
	
	temp[0] = (t->minute / 10) + 48;
	temp[1] = (t->minute % 10) + 48;
	draw_text(13, 0, temp, 5, 13, ORIENTATION_NORMAL, font_5x13, codepage_numbers, GRN_3, OVERLAY_REPLACE);
}


