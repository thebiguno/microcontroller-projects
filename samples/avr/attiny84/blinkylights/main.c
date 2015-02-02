/*
 * Toggles pin B4
 */

#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Do setup here
	DDRA = _BV(PORTA0);
	
	//Main program loop
	while (1){
		PORTA ^= _BV(PORTA0);
		_delay_ms(500);
	}
}

