#ifndef MANCHESTER_H
#define MANCHESTER_H

#include <avr/io.h>

// tx

void manchester_init_tx(volatile uint8_t *port, uint8_t pin, uint16_t baud);

void manchester_write(uint8_t data);

// rx

void manchester_init_rx(uint16_t baud);

uint8_t manchester_available();

uint8_t manchester_read(uint8_t *b);

#endif
