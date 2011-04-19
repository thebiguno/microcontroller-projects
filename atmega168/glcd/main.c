/*
 * PS2 Controller test.  Connect the wires as indicated in the psx_init() function call.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/glcd/glcd.h"
#include "lib/glcd/draw.h"
#include "lib/glcd/glcd_st7565r.h"
#include "lib/glcd/fonts/xlarge.h"
#include "lib/glcd/fonts/medium.h"

prog_uchar battery_0[] PROGMEM = {0x0e,0xfc,0x63,0x18,0xc6,0x3f};
prog_uchar battery_1[] PROGMEM = {0x0e,0xfc,0x63,0x18,0xc7,0xff};

int main (void){
	//Do setup here
//	_delay_ms(500);
	
	st7565r_init(&PORTD, PIND3, &PORTD, PIND2, &PORTD, PIND1, &PORTD, PIND0);
	
//	_delay_ms(500);
	
	/*
	glcd_draw_line(0, 0, 128, 32, OVERLAY_OR);
	glcd_draw_line(0, 32, 128, 0, OVERLAY_OR);
	
	glcd_draw_square(10, 10, 20, 20, 0, OVERLAY_OR);
	glcd_draw_square(100, 5, 110, 30, 1, OVERLAY_XOR);
	glcd_draw_square(40, 12, 70, 25, 1, OVERLAY_NAND);
	*/
//	glcd_draw_text(1, 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 5, 7, font_medium, codepage_medium, OVERLAY_OR);
//	glcd_draw_text(1, 10, "0", 11, 17, font_xlarge, codepage_xlarge, OVERLAY_OR);
	
	//glcd_draw_rectangle(0, 9, 128, 15, 1, OVERLAY_XOR);
	
	glcd_draw_bitmap(100, 15, 5, 9, battery_0, OVERLAY_OR);
	
	glcd_write_buffer();
	
	DDRB |= _BV(PINB0);
	
	glcd_set_contrast(0x20);
	
	while(1){
		PORTB = ~PORTB;
		_delay_ms(500);
	}
}
