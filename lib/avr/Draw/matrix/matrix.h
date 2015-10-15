#ifndef MATRIX_H
#define MATRIX_H

#ifndef MATRIX_WIDTH
#define MATRIX_WIDTH 24
#endif
#ifndef MATRIX_HEIGHT
#define MATRIX_HEIGHT 16
#endif

#ifndef MATRIX_DRIVER_SLAVE_ADDRESS
#define MATRIX_DRIVER_SLAVE_ADDRESS 42
#endif

#define MATRIX_LENGTH MATRIX_WIDTH * MATRIX_HEIGHT

#include <Draw.h>
#include "../../twi/twi.h"

namespace digitalcave {
	class Matrix : public Draw {
	private:
		uint8_t changed;
		uint8_t color;
		uint8_t depth;
		uint8_t buffer[MATRIX_WIDTH][MATRIX_HEIGHT];
		
	public:
		Matrix();
		~Matrix();
		
		void setPixel(int16_t x, int16_t y);
		uint8_t getPixel(int16_t x, int16_t y);
		uint8_t* getBuffer();

		void flush();
		
		/* value depends on depth */
		void setColor(uint8_t c);
		/* 0: 4 bit; 1: 2 bit */
		void setDepth(uint8_t d);
	};
}

#endif