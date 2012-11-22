/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/shiftcpp/shift.h"

void myCallback(){
	
}

int main (void){
	Shift shift(6);
	//shift.setCallback();
	shift.setLatch(&PORTB, PB3);
	
	uint8_t data[6];
	data[0] = 0xFF;
	data[1] = 0xFF;
	data[2] = 0xFF;
	data[3] = 0xFF;
	data[4] = 0xFF;
	data[5] = 0xFF;
	
	//Main program loop
	while (1){
		shift.shift(data);

		_delay_ms(50);
	}
}
