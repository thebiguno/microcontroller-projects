#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/serial/serial.h"
#include "lib/timer/timer.h"
#include <util/delay.h>
#include <avr/io.h>
#include "lib/draw/fonts/small.h"
#include "lib/draw/fonts/ascii.h"

int main (void){
	matrix_init();
	serial_init_b(9600);
	timer_init();

	//draw_text(0, 1, (char*) "Wyatt", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, 0x03, OVERLAY_OR);
	//draw_text(0, 9, (char*) "Monica", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, 0x03, OVERLAY_OR);
	
	/*
	draw_line(0, 0, 23, 15, 0x3, OVERLAY_OR);
	draw_line(0, 15, 23, 0, 0x3, OVERLAY_OR);
	
	set_pixel(23, 8, 0x3, OVERLAY_OR);
	set_pixel(22, 9, 0x3, OVERLAY_OR);
	set_pixel(21, 10, 0x3, OVERLAY_OR);
	set_pixel(20, 11, 0x3, OVERLAY_OR);
	set_pixel(19, 12, 0x3, OVERLAY_OR);
	set_pixel(18, 13, 0x3, OVERLAY_OR);
	set_pixel(17, 14, 0x3, OVERLAY_OR);
	set_pixel(16, 15, 0x3, OVERLAY_OR);
	*/
	
	char temp[6];

	while (1) {
		draw_rectangle(0, 1, 23, 7, DRAW_FILLED, 0xFF, OVERLAY_NAND);
		draw_text(0, 1, itoa((uint8_t) (timer_millis() / 1000), temp, 10), FONT_SMALL_WIDTH, FONT_SMALL_HEIGHT, ORIENTATION_NORMAL, font_small, codepage_ascii_caps, 0x03, OVERLAY_OR);
		_delay_ms(100);
	}
}
