/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/shift/shift.h"

int main (void){
	shift_init(&PORTB, PIND5, &PORTB, PIND7, &PORTB, PIND3);
	
	//Main program loop
	while (1){
		shift_out(0xFF);
		_delay_ms(50);
	}
}
