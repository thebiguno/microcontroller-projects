#include "Tictactoe.h"
#include "Matrix.h"
#include "lib/analog/analog.h"
#include "lib/draw/Rgb.h"
#include "lib/draw/Hsv.h"
#include "lib/Psx/Psx.h"
#include "lib/timer/timer.h"
#include <util/delay.h>
#include <stdlib.h>

using namespace digitalcave;

extern Matrix matrix;
extern Psx psx;
extern Hsv hsv;

Tictactoe::Tictactoe() {
}

Tictactoe::~Tictactoe() {
}

void Tictactoe::run() {
	_delay_ms(127);
	
	uint8_t running = 1;
	uint8_t blink = 0;
	
	uint8_t state[9] = {0,0,0, 0,0,0, 0,0,0};		// 0 = blank; 1 = p1; 2 = p2;
	uint8_t player = 1;

	uint8_t selection = 0;
	
	uint16_t buttons;
	
	Hsv comp = Hsv(hsv);
	comp.addHue(180);
	
	Rgb p1 = Rgb(hsv);
	Rgb p2 = Rgb(comp);
	
	while (running) {
		for (uint8_t i = 0; i < 9; i++) {
			uint8_t s = state[i];
			uint8_t x1 = (i % 3) * 4;
			uint8_t y1 = (i / 3) * 4;
			uint8_t x2 = x1 + 3;
			uint8_t y2 = y1 + 3;
			if (s == 0) {
				matrix.setColor(0,0,0);
			} else {
				matrix.setColor(s == 1 ? p1 : p2);
			}
			matrix.rectangle(x1,y1,x2,y2,DRAW_FILLED);
		}
		
		if (blink) {
			uint8_t x1 = (selection % 3) * 4;
			uint8_t y1 = (selection / 3) * 4;
			uint8_t x2 = x1 + 3;
			uint8_t y2 = y1 + 3;
			matrix.setColor(player == 1 ? p1 : p2);
			matrix.rectangle(x1,y1,x2,y2,DRAW_FILLED);
		}
		
		matrix.flush();
		
		blink++;
		blink %= 2;

		psx.poll();
		buttons = psx.buttons();
		if (buttons & PSB_PAD_UP || buttons & PSB_PAD_LEFT) {
			do {
				selection--;
				selection %= 9;
			} while (state[selection] != 0);
		}
		else if (buttons & PSB_PAD_DOWN || buttons & PSB_PAD_RIGHT) {
			do {
				selection++;
				selection %= 9;
			} while (state[selection] != 0);
		}
		else if (buttons & PSB_CROSS) {
			// set the state to the current player and change players
			state[selection] = player;
			player = (player == 1) ? 2 : 1;
			
			// find the next unfilled square
			selection = 9;
			for (uint8_t i = 0; i < 9; i++) {
				if (state[i] == 0) {
					selection = i;
					break;
				}
			}
			
			if (selection == 9) {
				// all the squares are filled, start a new game
				for (uint8_t i = 0; i < 9; i++) {
					state[i] = 0;
				}
				selection = 0;
			}
			
			// determine if there is a winner
			for (uint8_t p = 1; p < 3; p++) {
				if ((p == state[0] && p == state[1] && p == state[2]) ||
					(p == state[3] && p == state[4] && p == state[5]) ||
					(p == state[6] && p == state[7] && p == state[8]) ||
					(p == state[0] && p == state[3] && p == state[6]) ||
					(p == state[1] && p == state[4] && p == state[7]) ||
					(p == state[2] && p == state[5] && p == state[8]) ||
					(p == state[0] && p == state[4] && p == state[8]) ||
					(p == state[2] && p == state[4] && p == state[6])) {
				
					matrix.setColor(p == 1 ? p1 : p2);
					matrix.rectangle(0,0,11,11,DRAW_FILLED);
					
					matrix.flush();
					
					_delay_ms(255);
					
					for (uint8_t i = 0; i < 9; i++) {
						state[i] = 0;
					}
					selection = 0;
				}
			}
		}
		else if (buttons & PSB_TRIANGLE) {
			running = 0;
		}
		
		_delay_ms(127);
	}
}