/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/ShiftRegister/ShiftRegister.h"

using namespace digitalcave;
	
int main (void){
	//Do setup here
	
	// DDRC = _BV(PORTC5);
	ShiftRegister s(&PORTB, 3, 5);
	s.initLatch(&PORTB,PINB2);
	
	uint8_t data[3];
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	
	//Main program loop
	while (1){
		s.shift(data, 3);
		_delay_ms(500);
		data[2]++;
	}
}
