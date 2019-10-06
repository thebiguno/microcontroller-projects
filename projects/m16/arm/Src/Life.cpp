#include "Life.h"
#include "Matrix.h"
#include <stdlib.h>

using namespace digitalcave;

extern Matrix matrix;
extern Hsv hsv;

Life::Life(uint8_t w, uint8_t h): width(w), height(h) {
    state = new uint8_t[width * height];
}

Life::~Life() {
    delete [] state;
}

void Life::paint() {
    if (pause > 0) {
        pause--;
    } else if (frame > 0) {
        frame--;
    } else {
        uint8_t tempstate[width * height];

        frame = 5;  // repaint every 50 ms

        for (uint8_t x = 0; x < width; x++) {
            for (uint8_t y = 0; y < height; y++) {
                tempstate[x * width + y] = state[x * width + y];
            }
        }
        for (uint8_t x = 0; x < width; x++) {
            for (uint8_t y = 0; y < height; y++) {
                uint8_t count = getNeighborCount(x, y);
                if (state[x * width + y] > 0) {
                    // alive
                    if (count == 2 || count == 3) {
                        // staying alive; do nothing
                        tempstate[x * width + y] = 0x01;
                    }
                    else {
                        // overpopulation or underpopulation
                        tempstate[x * width + y] = 0x00;
                    }
                }
                else if (count == 3) {
                    // birth
                    tempstate[x * width + y] = 0x01;
                }
            }
        }
        for (uint8_t x = 0; x < width; x++) {
            for (uint8_t y = 0; y < height; y++) {
                state[x * width + y] = tempstate[x * width + y];
            }
        }

        //Store board hash
        for (uint8_t i = LIFE_HASH_COUNT - 1; i > 0; i--) {
            hashes[i] = hashes[i - 1];
        }
        hashes[0] = getStateHash();

        uint8_t m = 0;
        for (uint8_t i = 0; i < LIFE_HASH_COUNT; i++) {
            for (uint8_t j = i + 1; j < LIFE_HASH_COUNT; j++) {
                if (hashes[i] == hashes[j]) m++;
            }
        }
        if (m == 0) matches = 0;
        else matches++;

        if (matches >= LIFE_MATCH_COUNT) {
            reset();
            pause = 100; // 1s
        }

        flush();
    }
}

uint8_t Life::getState(int8_t x, int8_t y) {
    if (x < 0) x = width - 1;
    else if (x > width - 1) x = 0;
    if (y < 0) y = height - 1;
    else if (y > height - 1) y = 0;
    return state[x * width + y];
}

uint8_t Life::getNeighborCount(int8_t x, int8_t y) {
    uint8_t count = 0;
    if (getState(x - 1, y - 1)) count++;
    if (getState(x - 1, y)) count++;
    if (getState(x - 1, y + 1)) count++;
    if (getState(x, y - 1)) count++;
    if (getState(x, y + 1)) count++;
    if (getState(x + 1, y - 1)) count++;
    if (getState(x + 1, y)) count++;
    if (getState(x + 1, y + 1)) count++;
    return count;
}

uint32_t Life::getStateHash() {
    uint32_t hash = 0;
    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++) {
            hash += x * y * (state[x * width + y] ? 1 : 0);
        }
    }
    return hash;
}

void Life::flush() {
    hsv.addHue(1);
    Rgb rgb = Rgb(hsv);
    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++) {
            if (state[x * width + y] > 0) {
                matrix.setColor(rgb);
            } else {
                matrix.setColor(0,0,0);
            }
            matrix.setPixel(x, y);
        }
    }
    matrix.flush();
}

void Life::reset() {
    for (uint8_t i = 0; i < LIFE_HASH_COUNT; i++) {
        hashes[i] = 0;
    }
    matches = 0;

    // random start positions
    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++) {
            if ((random() & 0x3) == 0x3) {        //25% chance
                state[x * width + y] = 0x01;
            } else {
                state[x * width + y] = 0x00;
            }
        }
    }
}
