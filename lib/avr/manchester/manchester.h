#ifndef MANCHESTER_H
#define MANCHESTER_H

#include <avr/io.h>

// tx

void manchester_init_tx(volatile uint8_t *port, uint8_t pin, uint16_t baud);

/*
 * Writes length bytes from data.
 */
void manchester_write(uint8_t *data, uint8_t length);

// rx

void manchester_init_rx(uint16_t baud);

uint8_t manchester_available();

/*
 * Copies data from the buffer to the destination, up to a maximum of max.
 * Returns the number of bytes copied
 */ 
 uint8_t manchester_read(uint8_t *dst, uint8_t max);

#endif
