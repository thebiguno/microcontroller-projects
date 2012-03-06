#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

uint8_t display_next_shift();
void display_set_segments(char segments[], uint8_t flags);
void display_set_matrix(uint8_t red[], uint8_t grn[]);

#endif