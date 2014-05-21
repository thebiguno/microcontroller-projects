/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include "lib/analog/analog.h"

#define LATCH0		PORTB4
#define LATCH1		PORTB0

int main (void){
	//Set data bus and latch triggers as outputs
	DDRD = 0xFF;
	DDRB |= _BV(LATCH0) | _BV(LATCH1);
	
	//Enable analog on ADC pins 0 and 1
	uint8_t pins[2];
	pins[0] = 0;
	pins[1] = 1;
	analog_init(pins, 2, 0x00);

	uint16_t data;
	
	//Main program loop
	while (1){
		data = analog_read_p(0);
		PORTD = (data >> 2) & 0xFF;
		PORTB |= _BV(LATCH0);
		PORTB &= ~_BV(LATCH0);

		data = analog_read_p(1);
		PORTD = (data >> 2) & 0xFF;
		PORTB |= _BV(LATCH1);
		PORTB &= ~_BV(LATCH1);
	}
}

