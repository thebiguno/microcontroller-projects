#include "bitmaps.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include <util/delay.h>
#include <stdlib.h>

#define GRN_3	0x0C
#define GRN_2	0x08
#define GRN_1	0x04
#define RED_3	0x03
#define RED_2	0x02
#define RED_1	0x01

int main (void){
	
	matrix_init();
	matrix_set_mode(0x01);
	
	draw_bitmap(3, 0, PUMPKIN_WIDTH, PUMPKIN_HEIGHT, ORIENTATION_NORMAL, pumpkin,  RED_3 | GRN_3, OVERLAY_REPLACE);
	set_pixel(12, 1, GRN_1, OVERLAY_REPLACE);
	set_pixel(13, 1, GRN_1, OVERLAY_REPLACE);
	set_pixel(11, 2, GRN_1, OVERLAY_REPLACE);
	set_pixel(12, 2, GRN_1, OVERLAY_REPLACE);
	matrix_write_buffer();
	
	uint8_t d = 1;
	uint8_t c = 0x00;
	while (1) {
		if (d == 1) {
			if (c == 0x00) c = RED_1;
			else if (c == RED_1) c = RED_2;
			else { c = RED_3; d = 0; }
		} else {
			if (c == RED_3) c = RED_2;
			else if (c == RED_2) c = RED_1;
			else { c = 0x00; d = 1; }
		}

		draw_rectangle(8, 5, 9, 7, DRAW_FILLED, c, OVERLAY_REPLACE);
		draw_rectangle(14, 5, 15, 7, DRAW_FILLED, c, OVERLAY_REPLACE);
		matrix_write_buffer();
		
		_delay_ms(70);
	}
}