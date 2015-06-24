#include "Mood.h"
#include "lib/timer/timer.h"
#include "lib/draw/draw.h"
#include "Color.h"
#include <util/delay.h>
#include <stdlib.h>

using namespace digitalcave;

Plasma::Plasma() {
}

Plasma::~Plasma() {
}

void Plasma::run() {
	uint8_t running = 1;
	
	float hue;
	ws2812_t c;
	Color color;
	
	while (running > 0) {
		color.h2rgb(hue, c);
		draw_rectangle(0,0,11,11,DRAW_FILLED,c,DRAW_REPLACE);
		hue++;
		hue &= 360;
	}
}