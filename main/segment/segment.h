#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>
#include "../pins.h"

/*
 * Initializes the library to use the specified ports and pins for data out, clock, and latch.
 * You need to pass the ports by reference - e.g. shift_init(&PORTC, PORTC0, &PORTC, PORTC1, &PORTC, PORTC2).
 */
void segment_init(
		volatile uint8_t *data_port, uint8_t data_pin, 
		volatile uint8_t *clock_port, uint8_t clock_pin, 
		volatile uint8_t *latch_port, uint8_t latch_pin);

/*
 * Writes to a 4 digit 7-segment display.
 * dig1, dig2, dig3, dig4 are values for 0 to F to write to the digit
 * flag bits 0 - 4 are the four decimal points
 * flag bit 5 is L1L2
 * flag bit 6 is L3
 */
void segment_draw(char chars[], uint8_t flags);

#endif
