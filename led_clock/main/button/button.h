#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define BUTTON_MODE 0
#define BUTTON_HOUR 1
#define BUTTON_MIN 2

void button_init(volatile uint8_t *hour_port, uint8_t hour_pin, volatile uint8_t *min_port, uint8_t min_pin, volatile uint8_t *mode_port, uint8_t mode_pin);
void button_sample();
uint8_t button_state();
uint8_t button_changed();

#endif