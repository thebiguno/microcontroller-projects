#include "Life.h"
#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/timer/timer.h"
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
		
	}
}