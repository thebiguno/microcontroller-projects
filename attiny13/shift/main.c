/*
 * Test program to make sure programming has worked.  Setup by putting an LED
 * to ground (via a resistor) on pin 5.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "lib/shift/shift.h"

int main (void){
	//Do setup here
	
	//Set up PPORTB as output
	DDRB = 0xFF;
	
	int data = 16;
	int mode = -1;
	
	shift_init(&PORTB, PORTB0, &PORTB, PORTB1, &PORTB, PORTB2);
	
	//Main program loop
	while (1){
		shift_out(data);
		
		if (mode == -1) data = data >> 1;
		if (mode == 1) data = data << 1;
		if (data > 16){
			data = 16;
			mode = -1;
		}
		if (data < 1){
			data = 1;
			mode = 1;
		}
		
		//PORTB = ~PORTB;
		_delay_ms(100);
	}
}

