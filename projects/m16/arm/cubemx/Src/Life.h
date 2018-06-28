#ifndef Life_H
#define Life_H

#include "Module.h"
#include <stdint.h>

#define LIFE_HASH_COUNT  20
#define LIFE_MATCH_COUNT 20

namespace digitalcave {
    class Life : public Module {
    private:
        uint8_t width;
        uint8_t height;
        uint8_t state[][];
        uint8_t tempstate[][];
        uint32_t hashes[LIFE_HASH_COUNT];
        uint8_t matches;
        uint8_t frame;
        uint8_t pause;

    public:
        Life(uint8_t width, uint8_t height);
        ~Life();
        void paint();
        void reset();
    private:
        /* bounds-safe way of getting the current state of each pixel */
        uint8_t getState(int8_t x, int8_t y);

        uint8_t getNeighborCount(int8_t x, int8_t y);
        uint32_t getStateHash();
    
        /* write the board state to the matrix */
        void flush();
    };
}

#endif