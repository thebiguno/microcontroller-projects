#include "LedTable.h"

#include <stdlib.h>
#include <util/delay.h>
#include "lib/ws281x/ws2812.h"
#include "lib/draw/fonts/cp_ascii_caps.h"
#include "lib/draw/fonts/f_3x5.h"

#include "Matrix.h"
#include "lib/Psx/Psx.h"

#include "Life.h"
#include "Mood.h"
#include "Tictactoe.h"
/*
#include "Clock.h"
#include "AltClock.h"
#include "Tetris.h"
#include "Plasma.h"
*/

using namespace digitalcave;

Matrix matrix = Matrix();
Psx psx = Psx(&PORTF, 7, &PORTF, 5, &PORTF, 6, &PORTF, 4);
Hsv hsv = Hsv(0,255,30);

int main() {
	srandom(0);

	//timer_init();
	
	DDRB = 0xff;
	PORTB = 0x00;

	uint16_t buttons;
	uint8_t selected = 0;
	
	matrix.setFont(font_3x5, codepage_ascii_caps, 3, 5);
	
	
	matrix.setColor(64,64,64);
	matrix.rectangle(3,3,8,8, DRAW_FILLED);
	matrix.setColor(0,0,0);
	matrix.rectangle(5,5,6,6, DRAW_FILLED);
	matrix.setColor(192,192,192);
	matrix.rectange(0,0,2,2, DRAW_FILLED);
	matrix.rectange(9,0,11,2, DRAW_FILLED);
	matrix.rectange(0,9,2,11, DRAW_FILLED);
	matrix.rectange(9,9,11,11, DRAW_FILLED);
	matrix.setColor(255,255,255);
	matrix.setPixel(0,0);
	matrix.setPixel(0,11);
	matrix.setPixel(11,0);
	matrix.setPixel(11,11);
	matrix.setColor(128,255,128);
	matrix.line(3,0,4,0);
	matrix.setColor(192,255,128);
	matrix.line(5,0,6,0);
	matrix.setColor(255,255,128);
	matrix.line(7,0,8,0);

	
	
	while (1) {
		psx.poll();
		buttons = psx.buttons();
		if (buttons & PSB_PAD_UP) {
			selected++;
			selected %= 3;
		}
		else if (buttons & PSB_PAD_DOWN) {
			selected--;
			selected %= 3;
		}
		else if (buttons & PSB_L1) {
			hsv.addHue(-30);
		}
		else if (buttons & PSB_R1) {
			hsv.addHue(30);
		}
		else if (buttons & PSB_L2) {
			hsv.addValue(-15);
		}
		else if (buttons & PSB_R2) {
			hsv.addValue(15);
		}
		else if (buttons & PSB_CROSS) {
			switch (selected) {
				case 0: { Life life; life.run(); break; }
				case 1: { Tictactoe ttt; ttt.run(); break; }
				case 2: { Mood mood; mood.run(); break; }
/*
				case 0: { Clock clk; clk.run(); break; }
				case 1: { AltClock alt; alt.run(); break; }
				case 2: { Tictactoe ttt; ttt.run(); break; }
				case 3: { Tetris tet; tet.run; break; }
				case 4: { Life lif; lif.run(); break; }
				case 5: { Mood moo; moo.run(); break; }
				case 6: { Plasma pla; pla.run(); break; }
*/
			}
		}
		
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,11,11, DRAW_FILLED);
		matrix.setColor(Rgb(hsv));
		switch (selected) {
			case 0: matrix.text(0, 3, "LIF", DRAW_ORIENTATION_NORMAL); break;
			case 1: matrix.text(0, 3, "TET", DRAW_ORIENTATION_NORMAL); break;
			case 2: matrix.text(0, 3, "MOO", DRAW_ORIENTATION_NORMAL); break;
/*
			case 0: draw_text(0, 3, "CLK", DRAW_ORIENTATION_NORMAL); break;
			case 1: draw_text(0, 3, "ALT", DRAW_ORIENTATION_NORMAL); break;
			case 2: draw_text(0, 3, "TTT", DRAW_ORIENTATION_NORMAL); break;
			case 6: draw_text(0, 3, "PLA", DRAW_ORIENTATION_NORMAL); break;
*/
		}
		
		matrix.setPixel(4, 1);
		matrix.setPixel(5, 0);
		matrix.setPixel(6, 1);
		matrix.setPixel(4, 9);
		matrix.setPixel(5, 10);
		matrix.setPixel(6, 9);
		
		matrix.flush();
		
		_delay_ms(127);
	}
}
