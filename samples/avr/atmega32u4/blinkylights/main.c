/*
 * Toggles pin B4
 */

#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Do setup here
	DDRB = _BV(PORTB5);
	
	//Main program loop
	while (1){
		PORTB ^= _BV(PORTB5);
		_delay_ms(2000);
	}
}

