#ifndef Menu_H
#define Menu_H

#include <stdint.h>
#include "matrix.h"

namespace digitalcave {
	class Color {
	public:
		ws2812_t black;
		ws2812_t white;
		ws2812_t red;
		ws2812_t orange;
		ws2812_t yellow;
		ws2812_t chartreuse;
		ws2812_t green;
		ws2812_t spring;
		ws2812_t cyan;
		ws2812_t azure;
		ws2812_t blue;
		ws2812_t violet;
		ws2812_t magenta;
		ws2812_t rose;
		void h2rgb(float h, struct ws2812_t *rgb);
	}
}

#endif