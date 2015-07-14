#include "Mood.h"
#include "Matrix.h"
#include "lib/Draw/Hsv.h"
#include "lib/Draw/Rgb.h"
#include "lib/Psx/Psx.h"
#include <stdlib.h>
#include <util/delay.h>

using namespace digitalcave;

extern Hsv hsv;
extern Matrix matrix;
extern Psx psx;

Mood::Mood() {
}

Mood::~Mood() {
}

void Mood::run() {
	uint8_t running = 1;
	uint16_t buttons;
	
	Hsv c = Hsv(hsv);
	
	uint8_t i = 0;
	while (running > 0) {
		if (i++ == 0) {
			matrix.rectangle(0,0,11,11,DRAW_FILLED);
			matrix.flush();

			c.addHue(1);
			matrix.setColor(Rgb(c));
		}
		
		psx.poll();
		buttons = psx.buttons();
		if (buttons & PSB_TRIANGLE) {
			running = false;
		}
	}
}