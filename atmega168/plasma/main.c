#include "lib/draw/matrix/matrix.h"
#include "lib/twi/twi.h"

#include <util/delay.h>
#include <avr/wdt.h>

#define V_MAX 0.5

static uint8_t double_buffer[MATRIX_WIDTH][MATRIX_HEIGHT];

uint8_t get_double_buffer_value(uint8_t x, uint8_t y){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0;	//Bounds check
	return double_buffer[x][y];
}

void flush(){
    for (uint16_t i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; i++){
        ((uint8_t*) double_buffer)[i] = ((uint8_t*) matrix_get_working_buffer())[i];
    }
}

uint8_t max_adjacent_value(uint8_t x, uint8_t y){
	uint8_t max = 0;
	if (get_double_buffer_value(x - 1, y - 1) > max) 	max = get_double_buffer_value(x - 1, y - 1);
	if (get_double_buffer_value(x - 1, y) > max) 		max = get_double_buffer_value(x - 1, y);
	if (get_double_buffer_value(x - 1, y + 1) > max) 	max = get_double_buffer_value(x - 1, y + 1);
	if (get_double_buffer_value(x, y - 1) > max) 		max = get_double_buffer_value(x, y - 1);
	if (get_double_buffer_value(x, y + 1) > max) 		max = get_double_buffer_value(x, y + 1);
	if (get_double_buffer_value(x + 1, y - 1) > max) 	max = get_double_buffer_value(x + 1, y - 1);
	if (get_double_buffer_value(x + 1, y) > max) 		max = get_double_buffer_value(x + 1, y);
	if (get_double_buffer_value(x + 1, y + 1) > max) 	max = get_double_buffer_value(x + 1, y + 1);
	return max;
}

uint8_t fade_value(uint8_t v){
	switch (v){
		case 0xF0: return 0xE0;
		case 0xE0: return 0xD0;
		case 0xD0: return 0xC0;
		case 0xC0: return 0xB0;
		case 0xB0: return 0xA0;
		case 0xA0: return 0xA2;
		case 0xA2: return 0xA4;
		case 0xA4: return 0x95;
		case 0x95: return 0x86;
		case 0x86: return 0x77;
		case 0x77: return 0x68;
		case 0x68: return 0x59;
		case 0x59: return 0x4A;
		case 0x4A: return 0x3B;
		case 0x3B: return 0x3C;
		case 0x3C: return 0x3D;
		case 0x3D: return 0x2E;
		case 0x2E: return 0x1F;
		case 0x1F: return 0x0F;
		case 0x0F: return 0x0E;
		case 0x0E: return 0x0D;
		case 0x0D: return 0x0C;
		case 0x0C: return 0x0B;
		case 0x0B: return 0x0A;
		case 0x0A: return 0x09;
		case 0x09: return 0x08;
		case 0x08: return 0x07;
		case 0x07: return 0x06;
		case 0x06: return 0x05;
		case 0x05: return 0x04;
		case 0x04: return 0x03;
		case 0x03: return 0x02;
		case 0x02: return 0x01;
		case 0x01: return 0x00;
		default: return 0x00;
	}
}

int main (void){
	double_buffer[0][0] = 0x00;
	twi_init();
	twi_set_master_buffer(matrix_get_working_buffer());
	
	wdt_enable(WDTO_500MS);

	//x and y are locations of the moving pixel
	float x = MATRIX_WIDTH >> 1;
	float y = MATRIX_HEIGHT >> 1;
	//vx and vy are the velocities of the pixel, in the x and y components.
	float vx = (random() & 0x01 ? -1 : 1) * (random() & 0x03);
	float vy = (random() & 0x01 ? -1 : 1) * (random() & 0x03);
	
	while (1) {
		wdt_reset();
		//Move the pixel and adjust the velocities
		x += vx;
		y += vy;

		vx += (random() & 0x01 ? -0.01 : 0.01);
		vy += (random() & 0x01 ? -0.01 : 0.01);
					
		//Bounds checking x, y
		if (x >= MATRIX_WIDTH) x = 0;
		else if (x < 0) x = MATRIX_WIDTH - 1;
		if (y >= MATRIX_HEIGHT) y = 0;
		else if (y < 0) y = MATRIX_HEIGHT - 1;

		//Bounds checking vx, vy
		if (vx > V_MAX) x = V_MAX;
		else if (vx < -V_MAX) vx = -V_MAX;
		if (vy > V_MAX) vy = V_MAX;
		else if (vy < -V_MAX) vy = -V_MAX;


		//Fade everything
		for (uint8_t i = 0; i < MATRIX_WIDTH; i++){
			for (uint8_t j = 0; j < MATRIX_HEIGHT; j++){
				set_pixel(i, j, fade_value(get_double_buffer_value(i, j)), OVERLAY_REPLACE);
			}
		}
		
		flush();
		
		//Spread the love
		for (uint8_t i = 0; i < MATRIX_WIDTH; i++){
			for (uint8_t j = 0; j < MATRIX_HEIGHT; j++){
				set_pixel(i, j, max(fade_value(max_adjacent_value(i, j)), get_double_buffer_value(i, j)), OVERLAY_REPLACE);
			}
		}
		
		//Draw main pixel
		set_pixel(x, y, 0xF0, OVERLAY_REPLACE);

		flush();

		//Flush buffer
		twi_write_to(42, matrix_get_working_buffer(), MATRIX_WIDTH * MATRIX_HEIGHT, TWI_BLOCK, TWI_STOP);
		
		_delay_ms(100);
	}
}
