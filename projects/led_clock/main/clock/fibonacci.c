#include "clock.h"
#include "../lib/draw/draw.h"


void clock_draw(struct time_t *t) {
	//draw_rectangle(0, 0, 23, 15, DRAW_FILLED, 0x00, OVERLAY_REPLACE);
	
	uint8_t m = t->minute / 5;
	uint8_t h = t->hour % 12;
	
	uint8_t c5 = 0;
	uint8_t c3 = 0;
	uint8_t c2 = 0;
	uint8_t c1a = 0;
	uint8_t c1b = 0;
	
	if (h >= 5) c5 |= RED_3;
	if (h >= 8 || h == 3 || h == 4) c3 |= RED_3;
	if (h >= 10 || h == 7 || h == 2) c2 |= RED_3;
	if ((h & 1) == 1) c1a = RED_3;
	
	if (m >= 5) c5 |= GRN_3;
	if (m >= 8 || m == 3 || m == 4) c3 |= GRN_3;
	if (m >= 10 || m == 7 || m == 2) c2 |= GRN_3;
	if ((m & 1) == 1) c1b = GRN_3;
	
	draw_rectangle(6, 3, 8, 5, DRAW_FILLED, c1a, OVERLAY_REPLACE);
	draw_rectangle(6, 0, 8, 2, DRAW_FILLED, c1b, OVERLAY_REPLACE);
	draw_rectangle(0, 0, 5, 5, DRAW_FILLED, c2, OVERLAY_REPLACE);
	draw_rectangle(0, 6, 8, 14, DRAW_FILLED, c3, OVERLAY_REPLACE);
	draw_rectangle(9, 0, 23, 14, DRAW_FILLED, c5, OVERLAY_REPLACE);

}
