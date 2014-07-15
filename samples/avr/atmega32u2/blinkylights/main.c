/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Do setup here
	DDRB = _BV(PORTB4);
	
	//Main program loop
	while (1){
		PORTB ^= _BV(PORTB4);
		_delay_ms(2000);
	}
}

