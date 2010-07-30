/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/shift/shift.h"

int main (void){
	//Do setup here
	DDRC = 0xFF;
	
	serial_init(9600, 8, 0, 1);
	
	shift_init(&PORTC, PORTC0, &PORTC, PORTC1, &PORTC, PORTC2);
	
	int data = 16;
	int mode = -1;
	
	
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
