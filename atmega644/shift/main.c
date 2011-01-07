/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/shift/shift.h"

int main (void){
	shift_init(&PORTD, PIND4, &PORTD, PIND5, &PORTD, PIND6);
	
	int data = 1;
	int mode = 1;
	
	//Main program loop
	while (1){
		shift_out(data);
		
		if (mode == -1) data = data >> 1;
		if (mode == 1) data = data << 1;
		if (data == 128){
			mode = -1;
		}
		if (data == 1){
			mode = 1;
		}
		_delay_ms(50);
	}
}
