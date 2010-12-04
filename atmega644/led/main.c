/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Do setup here
	DDRB = 0xF;
	
	//Main program loop
	while (1){
		PORTB = ~PORTB;
		
		_delay_ms(500);
	}
}

