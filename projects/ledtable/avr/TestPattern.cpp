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
	
	Color color = Color(0);
	
	while (running > 0) {
		draw_set_value(color.rgb());
		draw_set_pixel(0, 0);
		color.addHue(1);
		draw_flush();
	}
}