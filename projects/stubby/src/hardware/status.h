#ifndef STATUS_H
#define STATUS_H

#include <avr/io.h>

#include "../Stubby.h"

#include "../hardware/pwm.h"

void status_init(volatile uint8_t **ports, uint8_t *pins);

void status_flash(uint8_t red, uint8_t green, uint8_t blue, uint8_t count);

void status_set_color(uint8_t red, uint8_t green, uint8_t blue);

#endif
