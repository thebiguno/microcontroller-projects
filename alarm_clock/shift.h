#ifndef SHIFT_H
#define SHIFT_H

#include <avr/io.h>

#define _BV32(bit) ((uint32_t) 1 << (bit)) 

#define SHIFT_PORT					PORTD
#define SHIFT_DATA_PIN				PIND5
#define SHIFT_CLOCK_PIN				PIND6
#define SHIFT_LATCH_PIN				PIND7

#define SHIFT_FLAG_COLON			_BV(0)
#define SHIFT_FLAG_PM				_BV(1)
#define SHIFT_FLAG_AUTO				_BV(2)

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
void shift_format_data(uint8_t hours, uint8_t minutes, uint8_t flags, uint32_t *data1, uint32_t *data2);

#endif