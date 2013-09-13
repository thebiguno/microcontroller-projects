/*
 * A program test test all of the modes of the matrix driver
 * 0x00: 8 bit color (blind copy of rx buffer data to internal buffer)
 * 0x01: 4 bit gradient mode
 * 0x02: 4 bit high contrast mode
 * 0x03: 2 bit, two color mode (R, Y, G, B, bright color)
 * 0x04: 2 bit, two color mode (R, Y, G, B, dim color)
 * 0x05: 2 bit green mode
 * 0x06: 2 bit yellow mode
 * 0x07: 2 bit red mode
 * 0x08: 1 bit green mode
 * 0x09: 1 bit yellow mode
 * 0x0A: 1 bit red mode
 * This uses the matrix LED board to display, and communicates with the board over twi.
 */

#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
// #include "lib/draw/fonts/large.h"
#include "lib/timer/timer.h"
#include <util/delay.h>

#define GRN_3	0xF0
#define GRN_2	0x80
#define GRN_1	0x40
#define RED_3	0x0F
#define RED_2	0x08
#define RED_1	0x04

void test_8bit(){
	char t[] = "8c";
	// draw_text(0,0,t,7,11,ORIENTATION_DOWN, font_large, codepage_large, 0xFF, OVERLAY_OR);
	matrix_set_mode(0x00);
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
			uint8_t c = y | (y << 4);
			if (x < 8) {
				c &= 0xF0;
			} else if (x < 16) {
				c &= 0x0F;
			}
			set_pixel(x, y, c, OVERLAY_REPLACE);
		}
	}
	matrix_write_buffer();
}
void test_4bit(){
	char t[] = "4c";
	// draw_text(0,0,t,7,11,ORIENTATION_DOWN, font_large, codepage_large, 0x0F, OVERLAY_OR);
	for (uint8_t m = 0x01; m < 0x03; m++) {
		matrix_set_mode(m);
		for (uint8_t v = 0x00; v < 0x0F; v++) {
			draw_line(0, 0, MATRIX_WIDTH-1, MATRIX_HEIGHT-1, 0xFF, OVERLAY_REPLACE);
//			draw_rectangle(0, 0, MATRIX_WIDTH-1, MATRIX_HEIGHT-1, DRAW_FILLED, v, OVERLAY_OR);
			matrix_write_buffer();
			_delay_ms(500);
		}
	}	
}
void test_2bit_color(){
	char t[] = "2c";
	// draw_text(0,0,t,7,11,ORIENTATION_DOWN, font_large, codepage_large, 0x03, OVERLAY_OR);
	for (uint8_t m = 0x03; m < 0x05; m++) {
		matrix_set_mode(m);
		for (uint8_t v = 0x00; v < 0x04; v++) {
			draw_line(0, 0, MATRIX_WIDTH-1, MATRIX_HEIGHT-1, 0xFF, OVERLAY_REPLACE);
//			draw_rectangle(0, 0, MATRIX_WIDTH-1, MATRIX_HEIGHT-1, DRAW_FILLED, v, OVERLAY_OR);
			matrix_write_buffer();
			_delay_ms(500);
		}
	}	
}
void test_2bit_mono(){
	char t[] = "2m";
	// draw_text(0,0,t,7,11,ORIENTATION_DOWN, font_large, codepage_large, 0x03, OVERLAY_OR);
	for (uint8_t m = 0x05; m < 0x08; m++) {
		matrix_set_mode(m);
		for (uint8_t v = 0x00; v < 0x04; v++) {
			set_pixel(v, v, 5, OVERLAY_REPLACE);
//			draw_line(0, 0, 5, 5, v, OVERLAY_REPLACE);
//			draw_rectangle(0, 0, MATRIX_WIDTH-1, MATRIX_HEIGHT-1, DRAW_FILLED, v, OVERLAY_OR);
			matrix_write_buffer();
			_delay_ms(1000);
		}
	}	
}
void test_1bit_mono(){
	char t[] = "1m";
	// draw_text(0,0,t,7,11,ORIENTATION_DOWN, font_large, codepage_large, 0x01, OVERLAY_OR);
	for (uint8_t m = 0x08; m < 0x0B; m++) {
		matrix_set_mode(m);
		draw_line(0, 0, MATRIX_WIDTH-1, MATRIX_HEIGHT-1, 0xFF, OVERLAY_REPLACE);
//		draw_rectangle(0, 0, MATRIX_WIDTH-1, MATRIX_HEIGHT-1, DRAW_FILLED, 1, OVERLAY_OR);
		matrix_write_buffer();
		_delay_ms(1000);
	}
}

int main (void){
	matrix_init();
	
	while (1) {
//		test_1bit_mono();  // modes 8,9,A
//		test_2bit_mono();  // modes 5,6,7
//		test_2bit_color(); // modes 3,4
//		test_4bit();       // modes 1,2
		test_8bit();       // mode  0
	}
}