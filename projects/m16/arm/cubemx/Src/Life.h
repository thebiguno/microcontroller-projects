#ifndef Life_H
#define Life_H

#include "Module.h"
#include <stdint.h>

#define LIFE_HASH_COUNT			20
#define LIFE_MATCH_COUNT		20

namespace digitalcave {
	class Life : public Module {
	private:
		uint8_t state[12][12];
		uint8_t tempstate[12][12];
		uint32_t hashes[LIFE_HASH_COUNT];
		uint8_t running = 0;
		uint8_t matches = 0;
	
	public:
		Life();
		~Life();
		void run();

	private:
		/* bounds-safe way of getting the current state of each pixel */
		uint8_t getState(int8_t x, int8_t y);

		uint8_t getNeighborCount(int8_t x, int8_t y);
		uint32_t getStateHash();
	
		/* write the board state to the matrix */
		void flush();
	
		/* remove hashes and randomize */
		void reset();
	};
}

#endif