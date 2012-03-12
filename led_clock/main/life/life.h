#ifndef LIFE_H
#define LIFE_H

#include <stdint.h>

void life_randomize();
void life_update();
void life_matrix(uint8_t red[], uint8_t grn[]);

#endif