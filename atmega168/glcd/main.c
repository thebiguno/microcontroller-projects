/*
 * PS2 Controller test.  Connect the wires as indicated in the psx_init() function call.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/glcd/glcd.h"
#include "lib/glcd/draw.h"
#include "lib/glcd/glcd_st7565r.h"


int main (void){
	//Do setup here
	_delay_ms(500);
	
	st7565r_init(&PORTD, PIND3, &PORTD, PIND2, &PORTD, PIND1, &PORTD, PIND0, &PORTD, PIND4);
	
	_delay_ms(500);
	
	
	glcd_draw_line(0, 0, 128, 32);
	glcd_draw_line(0, 32, 128, 0);
	
	glcd_draw_square(10, 10, 20, 20, 0);
	glcd_draw_square(100, 5, 110, 25, 1);
	
//	for (uint8_t i = 0; i < 6; i++){
//		glcd_set_pixel(i, 2, 1);
//	}

	glcd_write_buffer();
	
	DDRB |= _BV(PINB0);
	
	while(1){
		PORTB = ~PORTB;
		_delay_ms(500);
	}
}
