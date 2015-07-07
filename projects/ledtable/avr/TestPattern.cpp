#include "TestPattern.h"
#include "matrix.h"
#include <stdlib.h>

using namespace digitalcave;

TestPattern::TestPattern() {
}

TestPattern::~TestPattern() {
}

void TestPattern::run() {
	uint8_t running = 1;
	
	DDRB = 0xff;
	
	pixel_t black;
	pixel_t red;
	red.red = 255;
	
	while (running > 0) {
		for (uint8_t x = 0; x < 12; x++) {
			for (uint8_t y = 0; y < 12; y++) {
				draw_set_value(red);
				draw_set_pixel(x, y);
				//draw_flush();
				PORTB ^= PORTB;
				_delay_ms(60);
				draw_set_value(black);
				draw_set_pixel(x, y);
			}
		}
	}
}