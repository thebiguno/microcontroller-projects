#ifndef MATRIX_H
#define MATRIX_H

#include "lib/Draw/Draw.h"
#include "lib/Draw/Rgb.h"
#include "lib/ws281x/ws2812.h"

#define MATRIX_WIDTH 12
#define MATRIX_HEIGHT 12

namespace digitalcave {
	class Matrix : public Draw {
	private:
		uint8_t changed;
		ws2812_t buffer[MATRIX_WIDTH * MATRIX_HEIGHT];
		ws2812_t color;
		
	public:
		Matrix();
		~Matrix();
		
		void setPixel(int16_t x, int16_t y);
		void flush();
		
		void setColor(Rgb rgb);
		void setColor(uint8_t r, uint8_t g, uint8_t b);
	};
}

#endif