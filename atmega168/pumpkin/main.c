#include "bitmaps.h"
#include "lib/draw/fonts/large.h"
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
	
	while (1) {
		draw_bitmap(3, 0, PUMPKIN_WIDTH, PUMPKIN_HEIGHT, ORIENTATION_NORMAL, pumpkin,  RED_3 | GRN_3, OVERLAY_REPLACE);
		matrix_write_buffer();
		
		_delay_ms(70);
	}
}