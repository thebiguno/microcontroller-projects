#ifndef Tetris_H
#define Tetris_H

#include <Module.h>
#include <stdint.h>
#include "Matrix.h"

namespace digitalcave {
	class Tetris : public Module {
	private:
	uint16_t state[12][12];
	uint8_t piece; 			// 0 to 6
	uint8_t orientation;	// 0 to 3
	
	public:
		Tetris();
		~Tetris();
		void run();
	
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