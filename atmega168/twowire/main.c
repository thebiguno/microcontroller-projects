/*
 * Test program for TwoWire library.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lib/TwoWire/TwoWire.h"
#include "lib/serial/serial.h"

#define MASTER

#ifdef SLAVE
void receiveEvent(int howMany){
	uint8_t data = 0;
	while (Wire.available() > 0){
		data = Wire.receive();
		serial_write_c(data);
	}
	serial_write_s("\n\r");
}

int main (void){
	//Do setup here
	serial_init_b(9600);
	Wire.begin(42);
	Wire.onReceive(receiveEvent);
	DDRB |= _BV(PORTB0);
	
	//Main program loop
	while (1){
		PORTB = ~PORTB;
		_delay_ms(1000);
	}
}
#endif
#ifdef MASTER
int main (void){
	//Do setup here
	Wire.begin();
	DDRB |= _BV(PORTB0);
	sei();
	
	//Main program loop
	while (1){
		Wire.beginTransmission(42);
		Wire.send('a');
		Wire.endTransmission();
		PORTB = 0xFF;
		_delay_ms(1000);
	}
}
#endif