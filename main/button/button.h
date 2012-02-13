#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define BUTTON_HOUR 0x01
#define BUTTON_MIN 0x02
#define BUTTON_MODE 0x04

void button_init(volatile uint8_t *hour_port, uint8_t hour_pin, volatile uint8_t *min_port, uint8_t min_pin, volatile uint8_t *mode_port, uint8_t mode_pin);
void button_read(uint32_t ms);
uint8_t button_state();
uint8_t button_changed();

#endif