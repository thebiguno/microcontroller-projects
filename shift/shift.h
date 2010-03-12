#include <avr/io.h>

/*
 * Send a byte of data to a shift register, by defining the data and clock port / pin combination, as well as the
 * data byte.  Shifts MSB first.
 */
void shiftOut(volatile uint8_t *data_port, uint8_t data_pin, volatile uint8_t *clock_port, uint8_t clock_pin, volatile uint8_t *latch_port, uint8_t latch_pin, uint8_t data);

