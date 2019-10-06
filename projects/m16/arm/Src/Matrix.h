#ifndef MATRIX_H
#define MATRIX_H

#include <Draw.h>
#include <Rgb.h>

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 32

namespace digitalcave {
	class Matrix : public Draw {
	public:
		Matrix();
		~Matrix();

		void setPixel(int16_t x, int16_t y);
		void flush();

		void setColor(Rgb rgb);
		void setColor(uint8_t r, uint8_t g, uint8_t b);

	private:
		uint8_t changed;
		// frame buffer 15-bpp, [15 unused][14:10 red][9:5 green][4:0 blue]
		uint16_t buffer[MATRIX_WIDTH * MATRIX_HEIGHT]; // 2048 bytes
		uint16_t color;
	};
}

#endif
