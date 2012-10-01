#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/serial/serial.h"
#include "lib/timer/timer.h"
#include <util/delay.h>
#include <avr/io.h>
#include "lib/draw/fonts/tall.h"

int main (void){
	matrix_init();
	serial_init_b(9600);
	timer_init();

	//draw_text(0, 1, (char*) "Wyatt", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, 0x03, OVERLAY_OR);
	//draw_text(0, 9, (char*) "Monica", FONT_XSMALL_WIDTH, FONT_XSMALL_HEIGHT, ORIENTATION_NORMAL, font_xsmall, codepage_ascii_caps, 0x03, OVERLAY_OR);
	
	/*
	draw_line(0, 0, 23, 0, 0x3, OVERLAY_OR);
	draw_line(0, 1, 23, 1, 0x2, OVERLAY_OR);
	draw_line(0, 2, 23, 2, 0x1, OVERLAY_OR);
	draw_line(0, 0, 0, 15, 0xC, OVERLAY_OR);	
	draw_line(1, 0, 1, 15, 0x8, OVERLAY_OR);	
	draw_line(2, 0, 2, 15, 0x4, OVERLAY_OR);	

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
		/*
		draw_rectangle(0, 0, 23, 3, DRAW_FILLED, 0x03, OVERLAY_OR);
		draw_rectangle(0, 4, 23, 7, DRAW_FILLED, 0x02, OVERLAY_OR);
		draw_rectangle(0, 8, 23, 11, DRAW_FILLED, 0x01, OVERLAY_OR);
	
		draw_rectangle(0, 0, 5, 15, DRAW_FILLED, 0x0C, OVERLAY_OR);
		draw_rectangle(6, 0, 11, 15, DRAW_FILLED, 0x08, OVERLAY_OR);
		draw_rectangle(12, 0, 17, 15, DRAW_FILLED, 0x04, OVERLAY_OR);

		draw_text(0, 1, itoa((uint8_t) (timer_millis() / 1000), temp, 10), FONT_TALL_WIDTH, FONT_TALL_HEIGHT, ORIENTATION_NORMAL, font_tall, codepage_tall, 0x0F, OVERLAY_NAND);
		*/
		draw_rectangle(0, 0, 23, 15, DRAW_FILLED, 0xFF, OVERLAY_NAND);
		draw_text(0, 1, itoa((uint8_t) (timer_millis() / 1000), temp, 10), FONT_TALL_WIDTH, FONT_TALL_HEIGHT, ORIENTATION_NORMAL, font_tall, codepage_tall, RED_3, OVERLAY_OR);
		
		matrix_flush();
	}
}
