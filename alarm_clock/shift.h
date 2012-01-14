#ifndef SHIFT_H
#define SHIFT_H

#include <avr/io.h>

#define SHIFT_PORT					PORTD
#define SHIFT_DATA_PIN				PIND5
#define SHIFT_CLOCK_PIN				PIND6
#define SHIFT_LATCH_PIN				PIND7

/*
 * Initializes shift hardware
 */
void shift_init();

/*
 * Shift out the given byte to the shift register
 */
void shift_out(uint8_t data);

/*
 * Latch the data in the shift register to the output register
 */
void shift_latch();

/*
 * Given the information required, calculate the display buffer.
 */
void shift_format_data(uint8_t hours, uint8_t minutes, uint16_t *data1, uint16_t *data2);

#endif