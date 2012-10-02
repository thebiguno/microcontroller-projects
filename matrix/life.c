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

	setup();

	while (1) {
		for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
			for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
				uint8_t count = get_neighbor_count(x, y);
				uint8_t alive = matrix_get_display_pixel(x, y);
				if (alive) {
					if (count < 2) set_pixel(x, y, 0xF, OVERLAY_NAND);
					else if (count <= 3){
						if (alive == GRN_3) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, RED_3 | GRN_3, OVERLAY_OR);
						}
						else if (alive == (GRN_3 | RED_3)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, RED_3, OVERLAY_OR);
						}
					}
					else if (count > 3) set_pixel(x, y, 0xF, OVERLAY_NAND);
				}
				else if (count == 3){
					set_pixel(x, y, GRN_3, OVERLAY_OR);
				}
			}
		}
	
	/*
		set_pixel(0, 0, random() & 0xF, OVERLAY_OR);
		set_pixel(0, SIZE, random() & 0xF, OVERLAY_OR);
		set_pixel(SIZE, 0, random() & 0xF, OVERLAY_OR);
		set_pixel(SIZE, SIZE, random() & 0xF, OVERLAY_OR);

		for (uint8_t p = log10(SIZE) / log10(2); p > 0; p--){
			for (uint8_t x = 0; x <= SIZE; x += pow(2, p)){
				for (uint8_t y = 0; y <= SIZE; y += pow(2, p)){
					if (x % (uint8_t) pow(2, p + 1) == 0 && y % (uint8_t) pow(2, p + 1) == 0){
						; //Do nothing
					}
					else if (x % (uint8_t) pow(2, p + 1) == 0){
						uint8_t average = get_pixel(x, y + pow(2, p)) + get_pixel(x, y - pow(2, p)) / 2;
						uint8_t color = average + ROUGHNESS * random() & 0xFF / 128;		//TODO unsure if this is right; color = average + roughness * (RND - .5)
						set_pixel(x, y, 0xF, OVERLAY_NAND);	//Erase
						set_pixel(x, y, color, OVERLAY_OR);	//Draw
					}
					else if (y % (uint8_t) pow(2, p + 1) == 0){
						uint8_t average = get_pixel(x + pow(2, p), y) + get_pixel(x - pow(2, p), y) / 2;
						uint8_t color = average + ROUGHNESS * random() & 0xFF / 255;		//TODO unsure if this is right; color = average + roughness * (RND - .5)
						set_pixel(x, y, 0xF, OVERLAY_NAND);	//Erase
						set_pixel(x, y, color, OVERLAY_OR);	//Draw
					}
					else{ //if (x % pow(2, p + 1) > 0 && y % pow(2, p + 1) > 0)
						uint8_t v1 = get_pixel(x + pow(2, p), y + pow(2, p));
						uint8_t v2 = get_pixel(x + pow(2, p), x - pow(2, p));
						uint8_t v3 = get_pixel(x - pow(2, p), x + pow(2, p));
						uint8_t v4 = get_pixel(x - pow(2, p), y - pow(2, p));
						uint8_t average = (v1 + v2 + v3 + v4) / 4;
						uint8_t color = average + ROUGHNESS * random() & 0xFF / 255;
						set_pixel(x, y, 0xF, OVERLAY_NAND);	//Erase
						set_pixel(x, y, color, OVERLAY_OR);	//Draw
					}
				}
			}
		}
		*/
	
		/*
		draw_rectangle(0, 0, 23, 3, DRAW_FILLED, 0x03, OVERLAY_OR);
		draw_rectangle(0, 4, 23, 7, DRAW_FILLED, 0x02, OVERLAY_OR);
		draw_rectangle(0, 8, 23, 11, DRAW_FILLED, 0x01, OVERLAY_OR);
	
		draw_rectangle(0, 0, 5, 15, DRAW_FILLED, 0x0C, OVERLAY_OR);
		draw_rectangle(6, 0, 11, 15, DRAW_FILLED, 0x08, OVERLAY_OR);
		draw_rectangle(12, 0, 17, 15, DRAW_FILLED, 0x04, OVERLAY_OR);

		draw_text(0, 1, itoa((uint8_t) (timer_millis() / 1000), temp, 10), FONT_TALL_WIDTH, FONT_TALL_HEIGHT, ORIENTATION_NORMAL, font_tall, codepage_tall, 0x0F, OVERLAY_NAND);
		*/
		
		/*
		draw_rectangle(0, 0, 23, 15, DRAW_FILLED, 0xFF, OVERLAY_NAND);
		draw_text(0, 1, itoa((uint8_t) (timer_millis() / 1000), temp, 10), FONT_TALL_WIDTH, FONT_TALL_HEIGHT, ORIENTATION_NORMAL, font_tall, codepage_tall, RED_3, OVERLAY_OR);
		*/
		matrix_flush();
		
		_delay_ms(10);
		
		if (timer_millis() > 10000){
			timer_init();
			setup();
		}
	}
}
