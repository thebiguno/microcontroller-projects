/*
 * Sample skeleton source file.
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

int main (void){
	serial_init_b(BAUD);

	uint8_t i = 0;
	char c;
	DDRB |= _BV(PINB0);
	
	//Main program loop
	while (1){
		uint8_t b = serial_read_c(&c);
		if (i != b){
			PORTB |= _BV(PINB0);
			i = 0xFF;
			_delay_ms(100);
		}
		i++;
		PORTB &= ~_BV(PINB0);
		
	}
}
