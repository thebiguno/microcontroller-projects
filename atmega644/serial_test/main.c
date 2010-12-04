/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define BUF_SIZE 128

#include "lib/serial/serial.h"
int main (void){
	serial_init(57600, 8, 0, 1);
		
	uint8_t write = 0;
	uint8_t read = 0;
	char buffer[BUF_SIZE];
	char byte_buffer[2];

	DDRB |= _BV(PINB0) | _BV(PINB1) | _BV(PINB4);

	//Main program loop
	while (1){
		PORTB ^= _BV(PINB4);
		
		for (char c = 'A'; c < 'z'; c++){
			serial_write_c(c);
		}
	}
}

