#include "LedCubicle.h"

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#include "Matrix.h"

#include "Life.h"
#include "Plasma.h"

using namespace digitalcave;

Matrix matrix;
volatile uint8_t timer = 0;
uint8_t baseColor = 0; // 0 = green, 1 = yellow, 2 = red


//uint8_t wj[] PROGMEM = { 0x88, 0x0, 0x0, 0x88, 0x0, 0x0, 0x8b, 0x36, 0x4c, 0x88, 0xa4, 0xaa, 0xa9, 0xa4, 0xea, 0xaa, 0xa4, 0x8a, 0x51, 0xa4, 0x6a, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x2d, 0x36, 0xd3, 0x25, 0xa4, 0x9a, 0x2d, 0xb6, 0xdb, 0x2d, 0x92, 0x99, 0xcd, 0xb6, 0xdb, 0x0, 0x0, 0x0 };

int main() {
//	srandom(0);
	
	twi_init();
	
	uint8_t buffer[385];
	buffer[0] = 0;
	
	for (uint8_t i = 1; i < 385; i++) buffer[i] = 0xff;
	twi_write_to(42, buffer, 385, TWI_BLOCK, TWI_STOP);
	
	while (1) {
		;
		
		/*
		matrix.setColor(255,255);
		for (uint8_t x = 0; x < 8; x++) {
			for (uint8_t y = 0; y < 8; y++) {
				matrix.setPixel(x,y);
			}
		}
		matrix.flush();

		_delay_ms(1000);

		matrix.setColor(0,255);
		for (uint8_t x = 8; x < 16; x++) {
			for (uint8_t y = 0; y < 8; y++) {
				matrix.setPixel(x,y);
			}
		}

		matrix.setColor(255,0);
		for (uint8_t x = 16; x < 24; x++) {
			for (uint8_t y = 0; y < 8; y++) {
				matrix.setPixel(x,y);
			}
		}
		
		matrix.setColor(128,128);
		for (uint8_t x = 0; x < 8; x++) {
			for (uint8_t y = 8; y < 16; y++) {
				matrix.setPixel(x,y);
			}
		}

		matrix.setColor(0,128);
		for (uint8_t x = 8; x < 16; x++) {
			for (uint8_t y = 8; y < 16; y++) {
				matrix.setPixel(x,y);
			}
		}

		matrix.setColor(128,0);
		for (uint8_t x = 16; x < 24; x++) {
			for (uint8_t y = 8; y < 16; y++) {
				matrix.setPixel(x,y);
			}
		}
		*/
		
//		if (baseColor == 0) matrix.setColor(0,255);
//		else if (baseColor == 1) matrix.setColor(255,255);
//		else if (baseColor == 2) matrix.setColor(255,0);
//		matrix.bitmap(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, 0, wj);
//		matrix.setPixel(0,0);
	//	matrix.flush();
		_delay_ms(200);
		
/*

		Life life(baseColor);
		life.run();
		
		Plasma plasma(baseColor);
		plasma.run();
		
		baseColor++;
		if (baseColor > 2) baseColor = 0;
*/
	}
}
