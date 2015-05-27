/*
 * Toggles pin B4
 */

#include <avr/io.h>
#include <util/delay.h>

#include "lib/bootloader/bootloader.h"

int main (void){
	//Do setup here
	DDRB = _BV(PORTB5);
	
	uint8_t c = 0;
	
	//Main program loop
	while (1){
		PORTB ^= _BV(PORTB5);
		_delay_ms(500);
		c++;
		
		if (c > 10){
			bootloader_jump();
		}
	}
}

