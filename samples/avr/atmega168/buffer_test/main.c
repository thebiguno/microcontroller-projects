#define MATRIX_HEIGHT_SHIFT		0

#include "lib/draw/matrix/matrix.h"
#include "lib/twi/twi.h"

#include <util/delay.h>

int main (void){
	matrix_init();
	matrix_set_mode(0x00);
	
	uint8_t v = 0xFF;

	while (1) {
		for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
			for (uint8_t y = 0; y < (MATRIX_HEIGHT >> MATRIX_HEIGHT_SHIFT); y++){
				set_pixel(x, y, v & (x << 4 | y), OVERLAY_REPLACE);
			}
		}

		//Flush buffer
		matrix_write_buffer();
		
		if (v) _delay_ms(2000);
		else _delay_ms(500);
		
		v = ~v;
	}
}
