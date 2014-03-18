/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/shift/shift.h"

int main (void){
	//Do setup here
	
	DDRC = _BV(PORTC5);
	shift_init(&PORTD, PORTD2, &PORTD, PORTD3, &PORTD, PORTD4);
	
	uint8_t data = 0;
	
	
	//Main program loop
	while (1){
		data++;
		shift_out(data);		
		
		PORTC = ~PORTC;
		_delay_ms(100);
	}
}
