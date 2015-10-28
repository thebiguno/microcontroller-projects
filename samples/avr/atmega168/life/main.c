/*
 * An implementation of Conway's Game of Life.  This uses the matrix LED board to display, and 
 * communicates with the board over twi (using the new twi library).  The twi buffers are shared
 * with the display buffer to save memory.  This is a good, compact implementation which can
 * be used as a base for more complicated projects.
 */

#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/timer/timer.h"
//#include "lib/draw/fonts/cp_ascii_caps.h"
//#include "lib/draw/fonts/f_3x5.h"
#include <util/delay.h>
#include <stdlib.h>

#define GRN_3	0x0C
#define GRN_2	0x08
#define GRN_1	0x04
#define RED_3	0x03
#define RED_2	0x02
#define RED_1	0x01

#define RECENT_HASH_COUNT			50
#define RECENT_HASH_MATCH_COUNT			50

uint8_t nameplate[] PROGMEM = {0x88, 0x1, 0x10, 0x88, 0x1, 0x10, 0x88, 0x3, 0xb8, 0x8a, 0x99, 0x10, 0xaa, 0xa9, 0x10, 0xa9, 0xa9, 0x10, 0x50, 0x99, 0x10, 0x3, 0x0, 0x0, 0xe, 0x40, 0x0, 0x11, 0x40, 0x0, 0x11, 0x4c, 0xce, 0x11, 0x51, 0x29, 0x11, 0x59, 0x29, 0x11, 0x4d, 0x29, 0x11, 0x45, 0x29, 0xe, 0x58, 0xc9};

//We write to the scratch buffer, and then flush to the matrix buffer.  We have to
// do this so that we can check for the current neighborhood, even as we are scanning
// through rows.  If we just used the matrix buffer, then as soon as we write a value
// we corrupt the reading of this current generation.
static uint8_t scratch_buffer[MATRIX_WIDTH][MATRIX_HEIGHT];

static uint32_t recent_hashes[RECENT_HASH_COUNT];
static uint8_t recent_hash_match_count = 0;

void set_scratch(uint8_t x, uint8_t y, uint8_t value){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;	//Bounds check
	scratch_buffer[x][y] = value;
}

uint8_t get_wrapped_pixel(int8_t x, int8_t y){
	while (x < 0) x += MATRIX_WIDTH;
	while (x >= MATRIX_WIDTH) x -= MATRIX_WIDTH;
	while (y < 0) y += MATRIX_HEIGHT;
	while (y >= MATRIX_WIDTH) y -= MATRIX_HEIGHT;
	return get_pixel(x, y);
}

uint8_t get_neighbor_count(uint8_t x, uint8_t y){
	uint8_t count = 0;
	if (get_wrapped_pixel(x - 1, y - 1)) count++;
	if (get_wrapped_pixel(x - 1, y)) count++;
	if (get_wrapped_pixel(x - 1, y + 1)) count++;
	if (get_wrapped_pixel(x, y - 1)) count++;
	if (get_wrapped_pixel(x, y + 1)) count++;
	if (get_wrapped_pixel(x + 1, y - 1)) count++;
	if (get_wrapped_pixel(x + 1, y)) count++;
	if (get_wrapped_pixel(x + 1, y + 1)) count++;
	return count;
}

uint32_t get_board_hash(){
	uint32_t hash = 0;
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
			hash += x * y * (get_pixel(x, y) ? 1 : 0);
		}
	}
	
	return hash;
}

void clear_scratch(){
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
			set_scratch(x, y, 0);
		}
	}	
}

void flush(){
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
			set_pixel(x, y, scratch_buffer[x][y], OVERLAY_REPLACE);
		}
	}
}

void show_name(uint8_t value){
	draw_rectangle(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, DRAW_FILLED, 0, OVERLAY_REPLACE);
	draw_bitmap(0, 0, 24, 16, ORIENTATION_NORMAL, nameplate, value, OVERLAY_REPLACE);
	matrix_write_buffer();
}

void setup(){
	//Show name plate
	for(uint8_t i = 0; i < 10; i++){
		show_name(GRN_2); _delay_ms(100);
		show_name(GRN_3); _delay_ms(100);
		show_name(RED_1 | GRN_3); _delay_ms(100);
		show_name(RED_2 | GRN_3); _delay_ms(100);
		show_name(RED_3 | GRN_3); _delay_ms(100);
		show_name(RED_3 | GRN_2); _delay_ms(100);
		show_name(RED_3 | GRN_1); _delay_ms(100);
		show_name(RED_3); _delay_ms(100);
		show_name(RED_2); _delay_ms(100);
		show_name(RED_3); _delay_ms(100);
		show_name(RED_3 | GRN_1); _delay_ms(100);
		show_name(RED_3 | GRN_2); _delay_ms(100);
		show_name(RED_3 | GRN_3); _delay_ms(100);
		show_name(RED_2 | GRN_3); _delay_ms(100);
		show_name(RED_1 | GRN_3); _delay_ms(100);
		show_name(GRN_3); _delay_ms(100);
	}
	
	//Clear board
	clear_scratch();
	flush();
	matrix_write_buffer();
	_delay_ms(100);

	//Random start positions
	for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
		for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
			if ((random() & 0x3) == 0x3){		//25% chance
				set_scratch(x, y, GRN_1);
			}
		}
	}
	
	for (uint8_t i = 0; i < RECENT_HASH_COUNT; i++){
		recent_hashes[i] = i;
	}
	
	flush();
	matrix_write_buffer();
}

int main (void){
	timer_init();
	uint8_t analog_pins[1] = {5};
	analog_init(analog_pins, 1, ANALOG_INTERNAL);
	matrix_init();
	matrix_set_mode(MATRIX_MODE_2BIT);
	
	srandom(analog_read_p(0));
	
	setup();
		
	while (1) {
		for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
			for (uint8_t y = 0; y < MATRIX_HEIGHT; y++){
				uint8_t count = get_neighbor_count(x, y);
				uint8_t alive = get_pixel(x, y);
				if (alive) {
					if (count < 2) set_scratch(x, y, 0x00);
					else if (count <= 3){
						if (alive == GRN_1) {
							set_scratch(x, y, GRN_2);
						}
						if (alive == GRN_2) {
							set_scratch(x, y, GRN_3);
						}
						if (alive == GRN_3) {
							set_scratch(x, y, GRN_3 | RED_1);
						}
						else if (alive == (GRN_3 | RED_1)) {
							set_scratch(x, y, GRN_3 | RED_2);
						}
						else if (alive == (GRN_3 | RED_2)) {
							set_scratch(x, y, GRN_3 | RED_3);
						}
						else if (alive == (GRN_3 | RED_3)) {
							set_scratch(x, y, GRN_2 | RED_3);
						}
						else if (alive == (GRN_2 | RED_3)) {
							set_scratch(x, y, GRN_1 | RED_3);
						}
						else if (alive == (GRN_1 | RED_3)) {
							set_scratch(x, y, RED_3);
						}
						else if (alive == (RED_3)) {
							set_scratch(x, y, RED_2);
						}
						else if (alive == (RED_2)) {
							set_scratch(x, y, RED_1);
						}
					}
					else if (count > 3) set_scratch(x, y, 0x00);
				}
				else if (count == 3){
					set_scratch(x, y, GRN_1);
				}
			}
		}
		
		flush();
		matrix_write_buffer();
		
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
		
		_delay_ms(70);
	}
}