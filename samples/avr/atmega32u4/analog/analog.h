#ifndef ANALOG_H
#define ANALOG_H
#include <avr/io.h>

#define ANALOG_AREF			0
#define ANALOG_AVCC			1
#define ANALOG_INTERNAL_256	3


/*
 * Initializes analog hardware.  analog_pins is an array of pin numbers;
 * count is the length of this array.  This allows you to only init the 
 * analog pins that you need to use.
 *
 * The aref argument can be one of three things:
 *  ANALOG_AREF: Use AREF, internal Vref turned off
 *  ANALOG_AVCC: Use AVCC with external cap at AREF pin
 *  ANALOG_INTERNAL: Use internal 1.1V reference with external cap at AREF pin
 *  ANALOG_INTERNAL_256: Same as ANALOG_INTERNAL, but with 2.56v instead of 1.1v.
 * Depending on your AVR chip, you may only have a subset of these options.
 */

void analog_init(uint8_t analog_pins[], uint8_t count, uint8_t aref);

/*
 * Reads the current analog values into given data buffer.  Buffer must be at 
 * least as long as count in analog_init.
 */
void analog_read_a(uint16_t *data);

/*
 * Returns the current analog value for the given pin.
 */
uint16_t analog_read_p(uint8_t index);

#endif
