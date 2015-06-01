#include "Menu.h"
#include "lib/draw/draw.h"
#include "lib/psx/psx.h"
#include <stdlib.h>

using namespace digitalcave;

/*

+---------+---------+---------+---------+
|0 Analog |1        |2        |3 Hex    |
|  Clock  |         |         |  Clock  |
+---------+---------+---------+---------+
|4 Tic-   |5 Reversi|6 Connect|7        |
|  Tac-Toe|         |  Four   |         |
+---------+---------+---------+---------+
|8        |9        |A        |B        |
|         |         |         |         |
+---------+---------+---------+---------+
|C Life   |D Plasma |E        |F Mood   |
|         |         |         |  Light  |
+---------+---------+---------+---------+

*/

void Life::start() {
	uint16_t buttons;
	ws2812_t black = { .red = 0xff; .green = 0x00; .blue = 0x00; };
	ws2812_t red = { .red = 0xff; .green = 0x00; .blue = 0x00; };
	uint8_t selected;
	
	while (1) {
		void psx_read_gamepad();
		buttons = psx_buttons();
		if (buttons & PSB_PAD_UP) {
			if (selected == 0x0) selected = 0xc;
			else if (selected == 0x1) selected = 0xd;
			else if (selected == 0x2) selected = 0xe;
			else if (selected == 0x3) selected = 0xf;
			else selected -= 0x4;
		}
		else if (buttons & PSB_PAD_DOWN) {
			if (selected == 0xc) selected = 0x0;
			else if (selected == 0xd) selected = 0x1;
			else if (selected == 0xe) selected = 0x2;
			else if (selected == 0xf) selected = 0x3;
			else selected += 0x4;
		}
		else if (buttons & PSB_PAD_LEFT) {
			if (selected == 0x0) selected = 0x3;
			else if (selected == 0x4) selected = 0x7;
			else if (selected == 0x8) selected = 0xb;
			else if (selected == 0xc) selected = 0xf;
			else selected -= 1;
		}
		else if (buttons & PSB_PAD_RIGHT) {
			if (selected == 0x3) selected = 0x0;
			else if (selected == 0x7) selected = 0x4;
			else if (selected == 0xb) selected = 0x8;
			else if (selected == 0xf) selected = 0xc;
			else selected += 1;
		}
		else if (buttons & PSB_CIRCLE) {
			switch (selected) {
				case 0x0: AnalogClock clock;
				case 0x4: Tictactoe tictactoe;
				case 0xc: Life life;
			}
		}
		
		draw_rectangle(0, 0, 12, 12, DRAW_FILLED, black)
		uint8_t x = selection % 4;
		uint8_t y = selection / 4;
		draw_rectangle(x, y, x+3,y+3, DRAW_FILLED, c, DRAW_REPLACE);
		matrix_write_buffer();
	}
}