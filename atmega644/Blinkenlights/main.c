/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Do setup here
	DDRA = 0xF;
	
	//Main program loop
	while (1){
		PORTA = ~PORTA;
		
		_delay_ms(500);
	}
}

