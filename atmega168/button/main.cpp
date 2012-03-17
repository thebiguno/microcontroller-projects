/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <lib/button/button.h>
using namespace std;

int main (void){
	//Do setup here
	volatile uint8_t *ports[1];
	ports[0] = &PORTB;
	uint8_t pins[1];
	pins[0] = PORTB1;
	Button b (ports, pins);
	
	//Main program loop
	for (;;){
		b.poll();
		if (b.isPressed(0)){
			PORTB |= _BV(2);
		}
		else {
			PORTB &= ~_BV(2);
		}
	}
}

