#ifndef MANCHESTER_H
#define MANCHESTER_H

#include <avr/io.h>

void manchester_init_rx(uint16_t baud);

uint8_t manchester_available();

uint8_t manchester_read(uint8_t *b);

#endif
