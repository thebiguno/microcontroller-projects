#ifndef MATRIX_H
#define MATRIX_H

#include <Draw.h>
#include <Rgb.h>

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 32
#define BAM_SIZE (MATRIX_WIDTH + 1) * (MATRIX_HEIGHT / 2)

//[x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a]

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

		// 8 levels BAM of [x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a]
		// this maps directly onto GPIOC and is transferred using DMA
		// ck is driven by the same timer as DMA and not part of the BAM codes
		// two rows (top half, bottom half) are driven at a time
		// there are 8 unique values spread across 256 time slices
		uint16_t bam[BAM_SIZE * 8]; // 1056 * 16 = 8448 bytes
		uint8_t time; // there are 256 time slices

        // 2.2 gamma: round(((i/31)^2.2)*255)
        uint8_t gamma[32] = { 0, 0, 1, 2, 3, 5, 7, 10, 13, 17, 21, 26, 32, 38, 44, 52, 60, 68, 77, 87, 97, 108, 120, 132, 145, 159, 173, 188, 204, 220, 237, 255 };

		/* returns a value between 0 and 8 inclusive for the highest bit set, or 0 if no bit is set */
		uint8_t msb(uint8_t b);
		void buffer_to_bam();
		void initHardware();

		// callbacks
		void data_transmitted_handler_0(UART_HandleTypeDef *handle);
		void data_transmitted_handler_1(UART_HandleTypeDef *handle);
		void transmit_error_handler(DMA_HandleTypeDef *hdma);
	};
}

#endif
