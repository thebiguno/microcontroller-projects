#include "LedTable.h"

#include "lib/ws281x/ws2812.h"
#include "Color.h"
#include "lib/draw/fonts/cp_ascii_caps.h"
#include "lib/draw/fonts/f_3x5.h"
#include "matrix.h"
#include "lib/psx/psx.h"
#include <stdlib.h>

#include "TestPattern.h"
/*
#include "Clock.h"
#include "AltClock.h"
#include "Tictactoe.h"
#include "Tetris.h"
#include "Life.h"
#include "Mood.h"
#include "Plasma.h"
*/

using namespace digitalcave;

int main() {
	//timer_init();
	
	DDRB = 0xff;
	
	pixel_t black;
	pixel_t c;
	c.red = 0;
	c.green = 5;
	c.blue = 5;
	
	ws2812_t buf[144];
	
	for (uint8_t i = 0; i < 144; i++) {
		buf[i].blue = 5;
	}
	ws281x_set(buf);
	
	while (true) {
		;
		/*
		for (uint8_t x = 0; x < 12; x++) {
			for (uint8_t y = 0; y < 12; y++) {
				draw_set_value(c);
				draw_set_pixel(x, y);
				draw_flush();
				PORTB ^= 0x10;
				_delay_ms(50);
				draw_set_value(black);
				draw_set_pixel(x, y);
			}
		}
		*/
	}
	
	uint16_t buttons;
	uint8_t selected;
	Color color = Color(0);
	
	draw_set_font(font_3x5, codepage_ascii_caps, 3, 5);
	
	while (1) {
		void psx_read_gamepad();
		buttons = psx_buttons();
		if (buttons & PSB_PAD_UP) {
			selected++;
			if (selected > 6) selected = 0;
		}
		else if (buttons & PSB_PAD_DOWN) {
			selected--;
			if (selected > 6) selected = 6;
		}
		else if (buttons & PSB_CIRCLE) {
			switch (selected) {
				/*
				case 0: { Clock clk; clk.run(); break; }
				case 1: { AltClock alt; alt.run(); break; }
				case 2: { Tictactoe ttt; ttt.run(); break; }
				case 3: { Tetris tet; tet.run; break; }
				case 4: { Life lif; lif.run(); break; }
				case 5: { Mood moo; moo.run(); break; }
				case 6: { Plasma pla; pla.run(); break; }
				*/
				case 0: { TestPattern test; test.run(); break; }
			}
		}
		
		draw_set_value(color.rgb());
		
		switch (selected) {
			case 0: draw_text(0, 3, "CLK", DRAW_ORIENTATION_NORMAL); break;
			case 1: draw_text(0, 3, "ALT", DRAW_ORIENTATION_NORMAL); break;
			case 2: draw_text(0, 3, "TTT", DRAW_ORIENTATION_NORMAL); break;
			case 3: draw_text(0, 3, "TET", DRAW_ORIENTATION_NORMAL); break;
			case 4: draw_text(0, 3, "LIF", DRAW_ORIENTATION_NORMAL); break;
			case 5: draw_text(0, 3, "MOO", DRAW_ORIENTATION_NORMAL); break;
			case 6: draw_text(0, 3, "PLA", DRAW_ORIENTATION_NORMAL); break;
		}
		
		draw_set_pixel(4, 1);
		draw_set_pixel(5, 0);
		draw_set_pixel(6, 1);
		draw_set_pixel(4, 9);
		draw_set_pixel(5, 10);
		draw_set_pixel(6, 9);
		draw_flush();
	}
	
}
