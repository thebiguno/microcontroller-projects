#include "TestPattern.h"
#include "lib/draw/draw.h"
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
		set_pixel(0, 0, color.rgb(), OVERLAY_REPLACE);
		color.addHue(1);
	}
}