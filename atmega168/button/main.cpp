/*
 * Proof of concept C++ program, using a C++ library.
 * Hardware: RBBB or similar, with a button tied to ground on 
 * pin D9 (PORTB1).  A serial link is used to show button state.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <lib/button/button.h>
#include <lib/serial/serial.h>

int main (void){
	//Do setup here
	Button b (&PORTB, PORTB1, 8);
	serial_init_b(57600);

	//Main program loop
	for (;;){
		b.poll();
		if (b.isChanged()){
			if (b.isPressed()){
				serial_write_s((char*) "Pressed\r\n");
			}
			else {
				serial_write_s((char*) "Released\r\n");
			}
		}
	}
}

