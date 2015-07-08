#include "matrix.h"

uint8_t draw_changed;
ws2812_t draw_buffer[144];
ws2812_t draw_value;

void draw_set_value(ws2812_t value) {
	draw_value.red = value.red;
	draw_value.green = value.green;
	draw_value.blue = value.blue;
}

void draw_set_pixel(int16_t x, int16_t y) {
	int16_t i = (x & 0x01) ? (x * 12 + 11 - y) : (x * 12 + y);
//	uint8_t i = x * 12 + y;
	if (i >= 144 || i < 0) return;
	
	ws2812_t current;
	current.red = draw_buffer[i].red;
	current.green = draw_buffer[i].green;
	current.blue = draw_buffer[i].blue;

	uint8_t draw_overlay = draw_get_overlay();
	if (draw_overlay == DRAW_OVERLAY_REPLACE){
		draw_buffer[i].red   = draw_value.red;
		draw_buffer[i].green = draw_value.green;
		draw_buffer[i].blue  = draw_value.blue;
	}
	else if (draw_overlay == DRAW_OVERLAY_OR){
		draw_buffer[i].red   |= draw_value.red;
		draw_buffer[i].green |= draw_value.green;
		draw_buffer[i].blue  |= draw_value.blue;
	}
	else if (draw_overlay == DRAW_OVERLAY_NAND){
		draw_buffer[i].red   &= ~draw_value.red;
		draw_buffer[i].green &= ~draw_value.green;
		draw_buffer[i].blue  &= ~draw_value.blue;
	}
	else if (draw_overlay == DRAW_OVERLAY_XOR){
		draw_buffer[i].red   ^= draw_value.red;
		draw_buffer[i].green ^= draw_value.green;
		draw_buffer[i].blue  ^= draw_value.blue;
	}
	
	if (draw_buffer[i].red != current.red || draw_buffer[i].green != current.green || draw_buffer[i].blue != current.blue) draw_changed = 1;
}

void draw_flush(){
	if (draw_changed) {
		ws281x_set(draw_buffer);
		draw_changed = 0;
	}
}