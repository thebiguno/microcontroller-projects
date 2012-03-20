#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define BUTTON_PLAY_PAUSE 0
#define BUTTON_SLIDE_COUNT 1

void button_init(volatile uint8_t *play_pause_port, uint8_t play_pause_pin, volatile uint8_t *slide_count_port, uint8_t select_count_pin);
void button_sample();
uint8_t button_state();
uint8_t button_changed();

#endif