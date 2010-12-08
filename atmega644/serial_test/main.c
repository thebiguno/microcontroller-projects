/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "lib/serial/serial.h"

int main (void){
	serial_init(2400, 8, 0, 1);
		
	DDRB |= _BV(PINB0) | _BV(PINB1) | _BV(PINB4);

	char buf[1];

	//Main program loop
	while (1){
		PORTB ^= _BV(PINB4);
		
		if (serial_available()){
			if (serial_read_c(buf)){
				serial_write_c(buf[0]);
			} 
		}
		
		PORTB &= ~(_BV(PINB0) | _BV(PINB1));
		
		_delay_ms(1);
	}
}

