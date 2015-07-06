#include "LedTable.h"
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
	
	uint16_t buttons;
	uint8_t selected;
	Color color = Color(0);
	
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
		
		pixel_t c = color.rgb();
		
		switch (selected) {
			case 0: draw_text(0, 3, "CLK", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
			case 1: draw_text(0, 3, "ALT", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
			case 2: draw_text(0, 3, "TTT", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
			case 3: draw_text(0, 3, "TET", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
			case 4: draw_text(0, 3, "LIF", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
			case 5: draw_text(0, 3, "MOO", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
			case 6: draw_text(0, 3, "PLA", 3, 5, ORIENTATION_NORMAL, font_3x5, codepage_ascii_caps, c, OVERLAY_REPLACE); break;
		}
		
		set_pixel(4, 1, c, OVERLAY_REPLACE);
		set_pixel(5, 0, c, OVERLAY_REPLACE);
		set_pixel(6, 1, c, OVERLAY_REPLACE);
		set_pixel(4, 9, c, OVERLAY_REPLACE);
		set_pixel(5, 10, c, OVERLAY_REPLACE);
		set_pixel(6, 9, c, OVERLAY_REPLACE);
		matrix_write_buffer();
	}
	
}
