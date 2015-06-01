#include "Life.h"
#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/timer/timer.h"
#include <util/delay.h>
#include <stdlib.h>

using namespace digitalcave;

Tictactoe::Tictactoe() {
}

Tictactoe::~Tictactoe() {
}

void Tictactoe::run() {
	uint8_t running = 1;
	uint8_t blink;
	
	uint8_t state[3*3];		// 0 = blank; 1 = red; 2 = blue;
	uint8_t player;			// 1 = red; 2 = blue;

	uint8_t selection;
	
	uint16_t buttons;
	Color color;
	
	while (running) {
		for (uint8_t x = 0; x < 3; x++) {
			for (uint8_t y = 0; y < 3; y++) {
				uint8_t s = state[x * 3 + y]; 
				uint8_t x1 = x * 4;
				uint8_t y1 = y * 4;
				uint8_t x2 = x1 + 4;
				uint8_t y2 = y1 + 4;
				if (s == 1 || s == 2) {
					ws2812_t c = state == 1 ? color.red : color.cyan;
					draw_rectangle(x1,y1,x2,y2,DRAW_FILLED,c,DRAW_REPLACE);
				}
				else {
					ws2812_t c = color.black;
					draw_rectangle(x1,y1,x2,y2,DRAW_FILLED,c,DRAW_REPLACE);
				}
			}
		}
		
		if (blink) {
			uint8_t x1 = selection % 3;
			uint8_t y1 = selection / 3;
			uint8_t x2 = x1 + 4;
			uint8_t y2 = y1 + 4;
			ws2812_t c = player == 1 ? color.red : color.cyan;
			draw_rectangle(x1,y1,x2,y2,DRAW_FILLED,c,DRAW_REPLACE);
		}
		
		matrix_write_buffer();
		
		blink = blink == 0 ? 1 : 0;

		void psx_read_gamepad();
		buttons = psx_buttons();
		if (buttons & PSB_PAD_UP || buttons & PSB_LEFT) {
			do {
				selection -= 1;
				if (selection > 8) selection = 8;
			} while (state[selection] != 0)
		}
		else if (buttons & PSB_PAD_DOWN || buttons & PSB_RIGHT) {
			do {
				selection += 1;
				if (selection > 8) selection = 0;
			} while (state[selection] != 0)
		}
		else if (buttons & PSB_CIRCLE) {
			state[selection] = player;
			selection = 9;
			for (uint8_t i = 0; i < 9; i++) {
				if (state[i] == 0) selection = i;
			}
			
			if (selection == 9) {
				// all the squares are filled, start a new game
				for (uint8_t i = 0; i < 9; i++) {
					state[i] = 0;
					selection = 0;
				}
			}
			
			// determine if there is a winner
			
			
		}
		else if (buttons & PSB_TRIANGLE) {
			running = 0;
		}
		
		_delay_ms(70);
	}
}