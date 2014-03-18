/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Do setup here
	DDRB |= _BV(PINB0);
	
	//Main program loop
	while (1){
		PORTB = ~PORTB;
		_delay_us(10);
	}
}

