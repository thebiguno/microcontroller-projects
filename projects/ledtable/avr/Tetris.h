#ifndef Tetris_H
#define Tetris_H

#include <Module.h>
#include <Color.h>
#include <stdint.h>
#include "matrix.h"

namespace digitalcave {
	class Tetris : public Module {
	private:
	// each row is represented by 16 bits where 0 and is blank and 1 is filled
	// this is used to make determinations about collisions; the colors are not stored here but are bufferred in the matrix directly
	// the extra bits of the row are used as walls
	uint16_t state[12];
	ws2812_t current;
	Color color;
	
	// each piece is a 4x4 bitmap encoded in 16-bits and there are 4 rotations for each piece
	uint8_t pieces[56] PROGMEM = {
		0x06,0x60, 0x06,0x60, 0x06,0x60, 0x06,0x60, // O
		0x0f,0x00, 0x22,0x22, 0x0f,0x00, 0x22,0x22, // I
		0x03,0x60, 0x23,0x10, 0x03,0x60, 0x23,0x10, // S
		0x06,0x30, 0x13,0x20, 0x06,0x30, 0x13,0x20, // Z
		0x07,0x40, 0x22,0x30, 0x17,0x00, 0x62,0x20, // L
		0x07,0x10, 0x32,0x20, 0x47,0x00, 0x22,0x60, // J
		0x07,0x20, 0x23,0x20, 0x27,0x00, 0x26,0x20 // T
	};
	// each piece bitmap is painted in has a specific color
	ws2812_t colors[7] = {
		color.h2rgb(color.yellow),
		color.h2rgb(color.cyan),
		color.h2rgb(color.green),
		color.h2rgb(color.red),
		color.h2rgb(color.orange),
		color.h2rgb(color.blue),
		color.h2rgb(color.violet)
	};
		
	public:
		Tetris();
		~Tetris();
	
	private:
		uint8_t newGame();
		void generate();
		void rotate();
		void left();
		void right();
		void down();
		uint8_t collisionTest();
		void removeRow();
		void flush();
	};
}

#endif