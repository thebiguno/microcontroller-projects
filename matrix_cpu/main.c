#include "lib/draw/draw.h"
#include "lib/analog/analog.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/serial/serial.h"
#include "lib/timer/timer.h"
#include "lib/draw/fonts/tall.h"
#include "lib/Shift/Shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>


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
	Shift shift(&PORTB, PORTB0, &PORTB, PORTB1);

	uint8_t value[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

	while (1) {
		shift.send(value, 5);
	}

	/*
	serial_init_b(9600);
	timer_init();
	analog_init(analog_pins, 6, ANALOG_INTERNAL);

	setup();
	
	uint8_t sync_frame = 0x42;

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

		shift.send(&sync_frame, 1);
		shift.receive(matrix_get_working_buffer(), MATRIX_WIDTH * (MATRIX_HEIGHT >> 1));
		
		_delay_ms(10);
		
		if (timer_millis() > 10000){
			timer_init();
			setup();
		}
	}
	*/
}
