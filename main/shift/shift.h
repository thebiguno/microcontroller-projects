#ifndef SHIFT_H
#define SHIFT_H

#include <stdint.h>

/*
 * Initializes the library to use the specified ports and pins for data out, clock, and latch.
 * You need to pass the ports by reference - e.g. shift_init(&PORTB, PORTB3, &PORTB, PORTB2, &PORTB, PORTB5).
 * This library uses SPI so the data pin must be MOSI and the clock pin must SCK.
 */
void shift_init(
		volatile uint8_t *data_port, uint8_t data_pin, 
		volatile uint8_t *clock_port, uint8_t clock_pin, 
		volatile uint8_t *latch_port, uint8_t latch_pin);

void shift_enable();

void shift_data(uint8_t i, uint8_t t, uint8_t data);

void shift_do();

#endif