#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

#define BUTTON_HOUR 0x01
#define BUTTON_MIN 0x02
#define BUTTON_MODE 0x04

void button_init(volatile uint8_t *hr_port, uint8_t hr_pin, volatile uint8_t *mn_port, uint8_t mn_pin, volatile uint8_t *mode_port, uint8_t mode_pin);
uint8_t button_read();

#endif