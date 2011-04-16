/*
 * PS2 Controller test.  Connect the wires as indicated in the psx_init() function call.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/glcd/glcd.h"


int main (void){
	//Do setup here
	_delay_ms(500);
	
	glcd_init(&PORTD, PIND3, &PORTD, PIND2, &PORTD, PIND1, &PORTD, PIND0, &PORTD, PIND4);
	
	_delay_ms(500);
	
	do_something();
	
	
	DDRB |= _BV(PINB0);
	
	while(1){
		PORTB = ~PORTB;
		_delay_ms(500);
		
		do_something();
	}
}
