/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include "lib/Button/Buttons.h"

using namespace digitalcave;

int main (void){
	//Do setup here
	
	// read PD0 and PD1, sampled over 80 us
	Buttons buttons(&PORTD, 0x03, 1, 1);
	
	//Main program loop
	while (1) {
		uint8_t pressed = buttons.sample();
	}
}

