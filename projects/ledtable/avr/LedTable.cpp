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
#include "Tetris.h"
#include "Animated.h"
#include "AltClock.h"

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
	uint16_t changed;
	uint8_t selected = 0;
	
	matrix.setFont(font_3x5, codepage_ascii_caps, 3, 5);
	
	while (1) {
		psx.poll();
		buttons = psx.buttons();
		changed = psx.changed();
		if (buttons & PSB_SELECT && changed & PSB_SELECT) {
			selected++;
			selected %= 6;
		}
		else if (buttons & PSB_SQUARE && changed & PSB_SQUARE) {
			hsv.addHue(-30);
		}
		else if (buttons & PSB_CIRCLE && changed & PSB_CIRCLE) {
			hsv.addHue(30);
		}
		else if (buttons & PSB_PAD_DOWN && changed & PSB_PAD_DOWN) {
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
		else if (buttons & PSB_PAD_UP && changed & PSB_PAD_UP) {
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
		else if (buttons & PSB_PAD_LEFT && changed & PSB_PAD_LEFT) {
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
		else if (buttons & PSB_PAD_RIGHT && changed & PSB_PAD_RIGHT) {
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
		else if (buttons & PSB_START && changed & PSB_START) {
			switch (selected) {
				case 0: { Clock clk; clk.run(); break; }
				case 1: { AltClock alt; alt.run(); break; }
				case 2: { Life life; life.run(); break; }
				case 3: { Mood mood; mood.run(); break; }
				case 4: { Animated ani; ani.run(); break; }
				case 5: { Tictactoe ttt; ttt.run(); break; }
				case 6: { Tetris tet; tet.run(); break; }
			}
		}
		
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,11,11, DRAW_FILLED);
		matrix.setColor(Rgb(hsv));
		switch (selected) {
			case 0: matrix.text(0, 3, "CLK", DRAW_ORIENTATION_0); break;
			case 1: matrix.text(0, 3, "ALT", DRAW_ORIENTATION_0); break;
			case 2: matrix.text(0, 3, "LIF", DRAW_ORIENTATION_0); break;
			case 3: matrix.text(0, 3, "MOO", DRAW_ORIENTATION_0); break;
			case 4: matrix.text(0, 3, "ANI", DRAW_ORIENTATION_0); break;
			case 5: matrix.text(0, 3, "TTT", DRAW_ORIENTATION_0); break;
			case 6: matrix.text(0, 3, "TET", DRAW_ORIENTATION_0); break;
		}
		
		matrix.flush();
		
		_delay_ms(127);
	}
}
