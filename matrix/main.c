#include "lib/draw/draw.h"
#include "lib/analog/analog.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/serial/serial.h"
#include "lib/timer/timer.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include "lib/draw/fonts/tall.h"

#define SIZE 15
#define ROUGHNESS	16

uint8_t get_neighbor_count(uint8_t x, uint8_t y){
	uint8_t count = 0;
	if (matrix_get_display_pixel(x - 1, y - 1)) count++;
	if (matrix_get_display_pixel(x - 1, y)) count++;
	if (matrix_get_display_pixel(x - 1, y + 1)) count++;
	if (matrix_get_display_pixel(x, y - 1)) count++;
	if (matrix_get_display_pixel(x, y + 1)) count++;
	if (matrix_get_display_pixel(x + 1, y - 1)) count++;
	if (matrix_get_display_pixel(x + 1, y)) count++;
	if (matrix_get_display_pixel(x + 1, y + 1)) count++;
	return count;
}

void setup(){
	srandom(analog_read_p(0) * analog_read_p(1) * analog_read_p(2) * analog_read_p(3) * analog_read_p(4) * analog_read_p(5));

	for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
			if ((random() & 0x3) == 0x3){		//25% chance
				set_pixel(x, y, RED_3, OVERLAY_OR);
			}
		}
	}
	matrix_flush();
}

int main (void){
	uint8_t analog_pins[6] = {0, 1, 2, 3, 4, 5};
	matrix_init();
	serial_init_b(9600);
	timer_init();
	analog_init(analog_pins, 6, ANALOG_INTERNAL);

	char temp[6];

	while (1) {
		draw_rectangle(0, 0, 23, 3, DRAW_FILLED, 0x03, OVERLAY_OR);
		draw_rectangle(0, 4, 23, 7, DRAW_FILLED, 0x02, OVERLAY_OR);
		draw_rectangle(0, 8, 23, 11, DRAW_FILLED, 0x01, OVERLAY_OR);
	
		draw_rectangle(0, 0, 5, 15, DRAW_FILLED, 0x0C, OVERLAY_OR);
		draw_rectangle(6, 0, 11, 15, DRAW_FILLED, 0x08, OVERLAY_OR);
		draw_rectangle(12, 0, 17, 15, DRAW_FILLED, 0x04, OVERLAY_OR);

		draw_text(0, 1, itoa((uint8_t) (timer_millis() / 1000), temp, 10), FONT_TALL_WIDTH, FONT_TALL_HEIGHT, ORIENTATION_NORMAL, font_tall, codepage_tall, 0x0F, OVERLAY_NAND);
		
		/*
		draw_rectangle(0, 0, 23, 15, DRAW_FILLED, 0xFF, OVERLAY_NAND);
		draw_text(0, 1, itoa((uint8_t) (timer_millis() / 1000), temp, 10), FONT_TALL_WIDTH, FONT_TALL_HEIGHT, ORIENTATION_NORMAL, font_tall, codepage_tall, RED_3, OVERLAY_OR);
		*/
		matrix_flush();
	}
}
