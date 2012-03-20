#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

uint8_t display_next_shift();
void display_set_segments(uint8_t segments[], uint8_t flags);

#endif