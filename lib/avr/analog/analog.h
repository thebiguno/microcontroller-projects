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
#elif defined(__AVR_ATmega48__)   || \
	defined(__AVR_ATmega88__)   || \
	defined(__AVR_ATmega168__)   || \
	defined(__AVR_ATmega328__)     || \
	defined(__AVR_ATmega328P__)
	#define ANALOG_AREF		0
	#define ANALOG_AVCC		1
	#define ANALOG_INTERNAL	3
#elif defined(__AVR_ATmega324P__)  || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)
	#define ANALOG_AREF			0
	#define ANALOG_AVCC			1
	#define ANALOG_INTERNAL		2
	#define ANALOG_INTERNAL_256	3
#elif defined(__AVR_ATmega32U4__)
	#define ANALOG_AREF			0
	#define ANALOG_AVCC			1
	#define ANALOG_INTERNAL_256	3
#endif

#if defined (__cplusplus)
extern "C" {
#endif
	
/*
 * Initializes analog hardware.  analog_pins is an array of pin numbers;
 * count is the length of this array.  This allows you to only init the 
 * analog pins that you need to use.
 * 
 * Note that the pin numbers technically map to the MUX numbers of the ADC,
 * not strictly the ADC pin number itself.  For most chips the numbers are
 * the same; however, for the ATMega32u4 (among some others) the higher ADCs
 * have MUX numbers which do not match the ADC pin number.  For instance
 * on this chip ADC 13 is MUX 0x25.  To use ADC 13, you should pass in
 * 0x25 as the pin number.
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

#if defined (__cplusplus)
}
#endif
	
#endif
