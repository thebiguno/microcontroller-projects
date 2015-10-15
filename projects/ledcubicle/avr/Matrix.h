#ifndef MATRIX_H
#define MATRIX_H

#include <Draw.h>
#include "lib/twi.h"

#define MATRIX_WIDTH 24
#define MATRIX_HEIGHT 16
#define MATRIX_LENGTH MATRIX_WIDTH * MATRIX_HEIGHT

namespace digitalcave {
	class Matrix : public Draw {
	private:
		uint8_t changed;
		uint8_t buffer[MATRIX_WIDTH * MATRIX_HEIGHT];
		uint8_t color;
		uint8_t depth;
		
	public:
		Matrix();
		~Matrix();
		
		void setPixel(int16_t x, int16_t y);
		void flush();
		
		void setDepth(uint8_t d);
		void setColor(uint8_t gr);
		/* sets the color value to a scaled color */
		/* int 4 bit depth the value is divided by 16 (0-16 -> 0000, ... 240-255 -> 1111) */
		/* in 2 bit depth the value is divided 64 (0-63 -> 00, 64-127 -> 01, 128-191 -> 10, 192-255 -> 11) */
		void setColor(uint8_t r, uint8_t g);
	};
}

#endif