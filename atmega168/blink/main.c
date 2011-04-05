/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Do setup here
	DDRC = _BV(PINC5);
	
	//Main program loop
	while (1){
		PORTC = ~PORTC;
		_delay_ms(500);
	}
}

