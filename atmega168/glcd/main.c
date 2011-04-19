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
#include "lib/glcd/fonts/large.h"
#include "lib/glcd/fonts/medium.h"
#include "lib/glcd/fonts/tiny.h"

prog_uchar battery_0[] PROGMEM = {0x0e,0xfc,0x63,0x18,0xc6,0x3f};
prog_uchar battery_1[] PROGMEM = {0x0e,0xfc,0x63,0x18,0xc7,0xff};
prog_uchar battery_2[] PROGMEM = {0x0e,0xfc,0x63,0x18,0xff,0xff};
prog_uchar battery_3[] PROGMEM = {0x0e,0xfc,0x63,0x1f,0xff,0xff};
prog_uchar battery_4[] PROGMEM = {0x0e,0xfc,0x63,0xff,0xff,0xff};
prog_uchar battery_5[] PROGMEM = {0x0e,0xfc,0x7f,0xff,0xff,0xff};
prog_uchar battery_6[] PROGMEM = {0x0e,0xff,0xff,0xff,0xff,0xff};

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
//	glcd_draw_text(1, 10, "0123456789", 7, 9, font_large, codepage_large, OVERLAY_OR);
	
	glcd_draw_text(1, 10, "0123", 11, 17, font_xlarge, codepage_xlarge, OVERLAY_OR);
	
	//glcd_draw_rectangle(0, 9, 128, 15, 1, OVERLAY_XOR);
	
//	glcd_draw_bitmap(100, 15, 5, 9, battery_0, OVERLAY_OR);
	
//	glcd_write_buffer();
	
	DDRB |= _BV(PINB0);
	
//	glcd_set_contrast(0x20);
	
	uint8_t counter = 7;
	
	//glcd_invert_display();
	
	prog_uchar* battery = battery_6;
	while(1){
		PORTB = ~PORTB;
		_delay_ms(500);

		counter--;	
		if (counter == 0) battery = battery_0;
		if (counter == 1) battery = battery_1;
		if (counter == 2) battery = battery_2;
		if (counter == 3) battery = battery_3;
		if (counter == 4) battery = battery_4;
		if (counter == 5) battery = battery_5;
		if (counter == 6) battery = battery_6;
	
		glcd_draw_rectangle(98, 13, 106, 26, 1, OVERLAY_NAND);
		glcd_draw_bitmap(100, 15, 5, 9, battery, OVERLAY_OR);
		glcd_write_buffer();
		
		if (counter == 0) {
			counter = 7;
		}
	}
}
