/*
 * Simple one channel Oscilloscope.  Reads digital values and 
 * dumps to the serial port asyncronously.  Pin D2 and D3 are used, and
 * must both be connected together.  It is a good idea to use a pulldown
 * resistor on these as well, so that the pins don't float.
 *
 * Board setup:
 *  -VCC / GND
 *  -Probe on D2; D2 and D3 connected together, and 
 *  -Serial output connected
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

#include "lib/timer/timer.h"

int main (void){
	//Do setup here

	//Init subsystems
	serial_init_b(76800);
	timer_init();

	//We read the input from Port D2
	DDRD &= ~_BV(PIND2);
	
	//Set up pin D2 and D3 as an external interrupts
	EIMSK |= _BV(INT0) | _BV(INT1);  //Enable INT0 and INT1
	EICRA |= _BV(ISC01);	//Trigger INT0 on falling edge
	EICRA |= _BV(ISC11) | _BV(ISC10); //Trigger INT1 on rising edge
	
	//Enable interrupts
	sei();
	
	while (1){
		_delay_ms(1000);
	}
}

static inline void send(uint8_t value){
	uint32_t time = timer_micros();
	
	//Set LSB to value
	if (value) time |= 0x1;
	else time &= ~0x1;
	
	serial_write_c((char) 0x7F);
	serial_write_c((time >> 24) & 0xFF);
	serial_write_c((time >> 16) & 0xFF);
	serial_write_c((time >> 8) & 0xFF);
	serial_write_c((time) & 0xFF);
}

ISR(INT0_vect){
	send(0);
}

ISR(INT1_vect){
	send(1);
}