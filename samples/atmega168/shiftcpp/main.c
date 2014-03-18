/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/shiftcpp/shift.h"

int main (void){
	//Do setup here
	
	// DDRC = _BV(PORTC5);
	Shift s(3);
	s.setLatch(&PORTB,PINB2);
	
	// &PORTD, PORTD2, &PORTD, PORTD3, &PORTD, PORTD4
	
	uint8_t data[3];
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	
	
	//Main program loop
	while (1){
		s.shift(data);
		_delay_ms(500);
		data[2]++;
	}
}
