#ifndef ANALOG_H
#define ANALOG_H
#include <avr/io.h>

#if defined(__AVR_ATtiny13__) 
	#define ANALOG_AVCC		0
	#define ANALOG_INTERNAL	1
#elif defined(__AVR_ATtiny25__)   || \
	  defined(__AVR_ATtiny45__)   || \
	  defined(__AVR_ATtiny85__)
	#define ANALOG_AVCC		0
	#define ANALOG_AREF		1
	#define ANALOG_INTERNAL	2
	//TODO Add support for REFS2
#elif defined(__AVR_ATmega168__)   || \
	defined(__AVR_ATmega324P__)    || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)
	#define ANALOG_AREF		0
	#define ANALOG_AVCC		1
	#define ANALOG_INTERNAL	3
#endif


/*
 * Initializes analog hardware.  The aref argument can be one of three things:
 * 0, 1, or 3.  According to the ATmega168 datasheet, the meanings are:
 * 0: Use AREF, internal Vref turned off
 * 1: Use AVCC with external cap at AREF pin
 * 3: Use internal 1.1V reference with external cap at AREF pin
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
