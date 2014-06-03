#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>

void debug_int(uint8_t id, int32_t value);
void debug_double(uint8_t id, double value);
void pwm_set_phase_batch(uint8_t index, uint16_t phase);
#endif

