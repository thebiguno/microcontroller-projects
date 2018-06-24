#ifndef MATRIX_H
#define MATRIX_H

#include <Draw.h>
#include <Rgb.h>
#include "lib/ws2812.h"

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 32
#define BAM_SIZE (MATRIX_WIDTH + 1) * (MATRIX_HEIGHT / 2)

//[x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a]
#define R0 6
#define G0 7
#define B0 8
#define R1 10
#define G1 11
#define B1 12
#define ST 4

namespace digitalcave {
	class Matrix : public Draw {
	private:
		uint8_t changed;
		// frame buffer 12-bpp, [15:12 unused][11:8 red][7:4 green][3:0 blue]
		uint16_t buffer[MATRIX_WIDTH * MATRIX_HEIGHT]; // 4096 bytes
		uint16_t color;

		// 8 levels BAM of [x:x:x:b1:g1:r1:ck:b0:g0:r0:oe:st:d:c:b:a]
		// this maps directly onto GPIOC and is transferred using DMA
		// ck is driven at double the rate by dedicated timer and not part of the BAM codes
		// two rows (top half, bottom half) are driven at a time
		// there are 8 unique values spread across 256 time slices
		uint16_t bam[BAM_SIZE * 8]; // 16640 bytes
		uint8_t time; // there are 256 time slices

		uint8_t gamma[16] = { 0x00, 0x01, 0x02, 0x05, 0x10, 0x12, 0x1e, 0x29, 0x38, 0x4b, 0x61, 0x79, 0x95, 0xb5, 0xd9, 0xff };

		
	public:
		Matrix();
		~Matrix();
		
		void setPixel(int16_t x, int16_t y);
		void flush();
		
		void setColor(Rgb rgb);
		void setColor(uint8_t r, uint8_t g, uint8_t b);

	private:
		uint8_t time_index_to_bam_index(uint8_t time_index);
		void buffer_to_bam();
	};
}

#endif