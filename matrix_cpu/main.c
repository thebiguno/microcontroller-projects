#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/timer/timer.h"
#include "lib/twi/twi.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#define RECENT_HASH_COUNT			5
#define RECENT_HASH_MATCH_COUNT		20


uint16_t recent_hashes[RECENT_HASH_COUNT];
uint8_t recent_hash_match_count = 0;

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

uint16_t get_board_hash(){
	uint16_t hash = 0;
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
			hash += matrix_get_display_pixel(x, y) + x;
		}
	}
	
	return hash;
}

void flash_palette(uint8_t value){
	draw_rectangle(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, DRAW_FILLED, 0x0F, OVERLAY_NAND);
	draw_rectangle(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, DRAW_FILLED, value, OVERLAY_OR);
	matrix_flush(matrix_get_working_buffer(), matrix_get_display_buffer());
	twi_write_to(42, matrix_get_display_buffer(), 192, TWI_BLOCK, TWI_STOP);
}

void setup(){
	uint64_t analog = 0;
	analog_read_a((uint16_t*) analog);
	srandom(timer_micros() / analog * random());
	
	//Flash palette
	flash_palette(GRN_1); _delay_ms(100);
	flash_palette(GRN_2); _delay_ms(100);
	flash_palette(GRN_3); _delay_ms(100);
	flash_palette(RED_1 | GRN_3); _delay_ms(100);
	flash_palette(RED_2 | GRN_3); _delay_ms(100);
	flash_palette(RED_3 | GRN_3); _delay_ms(100);
	flash_palette(RED_3 | GRN_2); _delay_ms(100);
	flash_palette(RED_3 | GRN_1); _delay_ms(100);
	flash_palette(RED_3); _delay_ms(100);
	flash_palette(RED_2); _delay_ms(100);
	flash_palette(RED_1); _delay_ms(100);
	
	//Clear board
	draw_rectangle(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, DRAW_FILLED, 0x0F, OVERLAY_NAND);
	matrix_flush(matrix_get_working_buffer(), matrix_get_display_buffer());
	_delay_ms(100);

	for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
			if ((random() & 0x3) == 0x3){		//25% chance
				set_pixel(x, y, GRN_1, OVERLAY_OR);
			}
		}
	}
	
	for (uint8_t i = 0; i < RECENT_HASH_COUNT; i++){
		recent_hashes[i] = i;
	}
	
	matrix_flush(matrix_get_working_buffer(), matrix_get_display_buffer());
}

int main (void){
	timer_init();
	uint8_t analog_pins[1] = {5};
	analog_init(analog_pins, 1, ANALOG_INTERNAL);
	twi_init();
	twi_set_master_buffer(matrix_get_display_buffer());

	setup();
		
	while (1) {
		for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
			for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
				uint8_t count = get_neighbor_count(x, y);
				uint8_t alive = matrix_get_display_pixel(x, y);
				if (alive) {
					if (count < 2) set_pixel(x, y, 0xF, OVERLAY_NAND);
					else if (count <= 3){
						if (alive == GRN_1) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, GRN_2, OVERLAY_OR);
						}
						if (alive == GRN_2) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, GRN_3, OVERLAY_OR);
						}
						if (alive == GRN_3) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, GRN_3 | RED_1, OVERLAY_OR);
						}
						else if (alive == (GRN_3 | RED_1)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, GRN_3 | RED_2, OVERLAY_OR);
						}
						else if (alive == (GRN_3 | RED_2)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, GRN_3 | RED_3, OVERLAY_OR);
						}
						else if (alive == (GRN_3 | RED_3)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, GRN_2 | RED_3, OVERLAY_OR);
						}
						else if (alive == (GRN_2 | RED_3)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, GRN_1 | RED_3, OVERLAY_OR);
						}
						else if (alive == (GRN_1 | RED_3)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, RED_3, OVERLAY_OR);
						}
						else if (alive == (RED_3)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, RED_2, OVERLAY_OR);
						}
						else if (alive == (RED_2)) {
							set_pixel(x, y, 0xF, OVERLAY_NAND);
							set_pixel(x, y, RED_1, OVERLAY_OR);
						}
					}
					else if (count > 3) set_pixel(x, y, 0xF, OVERLAY_NAND);
				}
				else if (count == 3){
					set_pixel(x, y, GRN_1, OVERLAY_OR);
				}
			}
		}
		
		matrix_flush(matrix_get_working_buffer(), matrix_get_display_buffer());
		
		twi_write_to(42, matrix_get_display_buffer(), 192, TWI_BLOCK, TWI_STOP);
		
		//Store board hash
		for (uint8_t i = RECENT_HASH_COUNT - 1; i > 0; i--){
			recent_hashes[i] = recent_hashes[i - 1];
		}
		recent_hashes[0] = get_board_hash();
		
		uint8_t matches = 0;
		for (uint8_t i = 0; i < RECENT_HASH_COUNT; i++){
			for (uint8_t j = i + 1; j < RECENT_HASH_COUNT; j++){
				if (recent_hashes[i] == recent_hashes[j]) matches++;
			}
		}
		if (matches == 0) recent_hash_match_count = 0;
		else recent_hash_match_count++;
		
		if (recent_hash_match_count >= RECENT_HASH_MATCH_COUNT){
			setup();
		}
		
		_delay_ms(50);
	}
}
