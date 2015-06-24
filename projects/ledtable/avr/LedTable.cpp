#include "LedTable.h"
#include "lib/draw/fonts/cp_ascii_caps.h"
#include "lib/draw/fonts/f_3x5.h"
#include "matrix.h"
#include "lib/psx/psx.h"
#include <stdlib.h>

#include "Clock.h"
#include "AltClock.h"
#include "Tictactoe.h"
#include "Tetris.h"
#include "Life.h"
#include "Mood.h"
#include "Plasma.h"

using namespace digitalcave;

int main() {
	//timer_init();
	
	uint16_t buttons;
	uint8_t selected;
	Color color;
	
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
				case 0: { Clock clk; clk.run(); break; }
				case 1: { AltClock alt; alt.run(); break; }
				case 2: { Tictactoe ttt; ttt.run(); break; }
				case 3: { Tetris tet; tet.run; break; }
				case 4: { Life lif; lif.run(); break; }
				case 5: { Mood moo; moo.run(); break; }
				case 6: { Plasma pla; pla.run(); break; }
			}
		}
		
		switch (selected) {
			case 0: draw_text(0, 3, "CLK", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
			case 1: text = "ALT"; break;
			case 2: text = "TTT"; break;
			case 3: text = "TET"; break;
			case 4: text = "LIF"; break;
			case 5: text = "MOO"; break;
			case 6: text = "PLA"; break;
		}
		
		pixel_t c = color.h2rgb(0);
		draw_text(0, 3, text, 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE);
		set_pixel(4, 1, c, OVERLAY_REPLACE);
		set_pixel(5, 0, c, OVERLAY_REPLACE);
		set_pixel(6, 1, c, OVERLAY_REPLACE);
		set_pixel(4, 9, c, OVERLAY_REPLACE);
		set_pixel(5, 10, c, OVERLAY_REPLACE);
		set_pixel(6, 9, c, OVERLAY_REPLACE);
		matrix_write_buffer();
	}
	
}
