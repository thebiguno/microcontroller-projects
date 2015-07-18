#include "LedTable.h"

#include <stdlib.h>
#include <util/delay.h>
#include "lib/ws281x/ws2812.h"
#include "lib/draw/fonts/cp_ascii_caps.h"
#include "lib/draw/fonts/f_3x5.h"
#include "lib/twi/twi.h"

#include "Matrix.h"
#include "lib/Psx/Psx.h"

#include "Life.h"
#include "Mood.h"
#include "Tictactoe.h"
#include "Clock.h"
/*
#include "AltClock.h"
#include "Tetris.h"
#include "Plasma.h"
*/

using namespace digitalcave;

Matrix matrix = Matrix();
Psx psx = Psx(&PORTF, 7, &PORTF, 5, &PORTF, 6, &PORTF, 4);
Hsv hsv = Hsv(0,0xff,0x0f);

int main() {
	twi_init();
	srandom(0);

	//timer_init();
	
	DDRB = 0xff;
	PORTB = 0x00;

	uint16_t buttons;
	uint8_t selected = 0;
	
	matrix.setFont(font_3x5, codepage_ascii_caps, 3, 5);
	
	while (1) {
		psx.poll();
		buttons = psx.buttons();
		if (buttons & PSB_PAD_UP) {
			selected++;
			selected %= 4;
		}
		else if (buttons & PSB_PAD_DOWN) {
			if (selected == 0) {
				selected = 3;
			} else {
				selected--;
				selected %= 4;
			}
		}
		else if (buttons & PSB_L3) {
			hsv.addHue(-30);
		}
		else if (buttons & PSB_R3) {
			hsv.addHue(30);
		}
		else if (buttons & PSB_L2) {
			uint8_t v = hsv.getValue();
			switch (v) {
				case 0xff: hsv.setValue(0x7f); break;
				case 0x7f: hsv.setValue(0x3f); break;
				case 0x3f: hsv.setValue(0x1f); break;
				case 0x1f: hsv.setValue(0x0f); break;
				case 0x0f: hsv.setValue(0x07); break;
				case 0x07: hsv.setValue(0x03); break;
				case 0x03: hsv.setValue(0x01); break;
				default: hsv.setValue(0x00);
			}
		}
		else if (buttons & PSB_R2) {
			uint8_t v = hsv.getValue();
			switch (v) {
				case 0x00: hsv.setValue(0x01); break;
				case 0x01: hsv.setValue(0x03); break;
				case 0x03: hsv.setValue(0x07); break;
				case 0x07: hsv.setValue(0x0f); break;
				case 0x0f: hsv.setValue(0x1f); break;
				case 0x1f: hsv.setValue(0x3f); break;
				case 0x3f: hsv.setValue(0x7f); break;
				default: hsv.setValue(0xff);
			}
		}
		else if (buttons & PSB_L1) {
			uint8_t s = hsv.getSaturation();
			switch (s) {
				case 0xff: hsv.setSaturation(0xfe); break;
				case 0xfe: hsv.setSaturation(0xfc); break;
				case 0xfc: hsv.setSaturation(0xf8); break;
				case 0xf8: hsv.setSaturation(0xf0); break;
				case 0xf0: hsv.setSaturation(0xe0); break;
				case 0xe0: hsv.setSaturation(0xc0); break;
				case 0xc0: hsv.setSaturation(0x80); break;
				default: hsv.setSaturation(0x00);
			}
		}
		else if (buttons & PSB_R1) {
			uint8_t s = hsv.getSaturation();
			switch (s) {
				case 0x00: hsv.setSaturation(0x80); break;
				case 0x80: hsv.setSaturation(0xc0); break;
				case 0xc0: hsv.setSaturation(0xe0); break;
				case 0xe0: hsv.setSaturation(0xf0); break;
				case 0xf0: hsv.setSaturation(0xf8); break;
				case 0xf8: hsv.setSaturation(0xfc); break;
				case 0xfc: hsv.setSaturation(0xfe); break;
				default: hsv.setSaturation(0xff);
			}
		}
		else if (buttons & PSB_CROSS) {
			switch (selected) {
				case 0: { Life life; life.run(); break; }
				case 1: { Tictactoe ttt; ttt.run(); break; }
				case 2: { Mood mood; mood.run(); break; }
				case 3: { Clock clk; clk.run(); break; }
/*
				case 1: { AltClock alt; alt.run(); break; }
				case 3: { Tetris tet; tet.run; break; }
				case 6: { Plasma pla; pla.run(); break; }
*/
			}
		}
		
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,11,11, DRAW_FILLED);
		matrix.setColor(Rgb(hsv));
		switch (selected) {
			case 0: matrix.text(0, 3, "LIF", DRAW_ORIENTATION_NORMAL); break;
			case 1: matrix.text(0, 3, "TTT", DRAW_ORIENTATION_NORMAL); break;
			case 2: matrix.text(0, 3, "MOO", DRAW_ORIENTATION_NORMAL); break;
			case 3: matrix.text(0, 3, "CLK", DRAW_ORIENTATION_NORMAL); break;
/*
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
