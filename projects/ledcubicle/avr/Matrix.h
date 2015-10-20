#ifndef MATRIX_H
#define MATRIX_H

#include <Draw.h>
#include "lib/twi.h"

#define MATRIX_WIDTH 24
#define MATRIX_HEIGHT 16
#define MATRIX_BUFFER_LENGTH MATRIX_WIDTH * MATRIX_HEIGHT + 1
#define MATRIX_DRIVER_SLAVE_ADDRESS 42

namespace digitalcave {
	class Matrix : public Draw {
	private:
		uint8_t changed;
		uint8_t buffer[MATRIX_BUFFER_LENGTH];
		uint8_t color;
		
	public:
		Matrix();
		~Matrix();
		
		void setPixel(int16_t x, int16_t y);
		void flush();
		
		void setDepth(uint8_t d);
		void setColor(uint8_t gr);
		void setColor(uint8_t r, uint8_t g);
	};
}

#endif