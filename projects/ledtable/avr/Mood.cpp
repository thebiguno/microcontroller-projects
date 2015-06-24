#include "Mood.h"
#include "lib/draw/draw.h"
#include "Color.h"
#include <stdlib.h>

using namespace digitalcave;

Mood::Mood() {
}

Mood::~Mood() {
}

void Mood::run() {
	uint8_t running = 1;
	
	uint16_t h;
	Color color;
	
	while (running > 0) {
		draw_rectangle(0,0,11,11,DRAW_FILLED,color.rgb(),DRAW_REPLACE);
		h++;
		h &= 360;
		color.setHue(h);
	}
}