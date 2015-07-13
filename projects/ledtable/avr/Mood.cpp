#include "Mood.h"
#include "Matrix.h"
#include "lib/Draw/Hsv.h"
#include "lib/Draw/Rgb.h"
#include "lib/Psx/Psx.h"
#include <stdlib.h>
#include <util/delay.h>

using namespace digitalcave;

extern Matrix matrix;
extern Psx psx;

Mood::Mood() {
}

Mood::~Mood() {
}

void Mood::run() {
	uint8_t running = 1;
	uint16_t buttons;
	
	Hsv hsv = Hsv(0,255,127);
	
	while (running > 0) {
		matrix.rectangle(0,0,11,11,DRAW_FILLED);
		matrix.flush();
		
		_delay_ms(127);
		
		hsv.addHue(1);
		matrix.setColor(Rgb(hsv));
		
		psx.poll();
		buttons = psx.buttons();
		if (buttons & PSB_TRIANGLE) {
			running = false;
		}
	}
}