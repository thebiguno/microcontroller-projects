#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lib/usb/serial.h"

/*
 * Demonstrate how to read an encoder.  For mechanical encoders (the nice and cheap ones) you will 
 * need a 1uF or larger capacitor to debounce each of the input lines.  Apparently optical encoders
 * do not need this, although I don't have any so did not confirm.
 *
 * When reading the encoder input, turning in one direction will blink the red light, turning in
 * the other direction will blink the green light.
 */

volatile int8_t encoder1_movement = 0;

int main (void){
	//Do setup here
	PCICR |= _BV(PCIE0);
	PCMSK0 |= 0x03;														//Enable bits 0..3 for pin change interrupts
	PORTB |= _BV(PORTB0) | _BV(PORTB1);									//Pullups on 0..3
	sei();
	
	usb_init();

	//Main program loop
	while (1){
		if (encoder1_movement != 0){
			char buf[10];
			uint8_t s = snprintf(buf, sizeof(buf), "%d\r\n", encoder1_movement);
			usb_serial_write((const uint8_t*) buf, s);
			encoder1_movement = 0;
		}
		_delay_ms(100);		//Update frequency of 10 Hz
	}
}

ISR(PCINT0_vect){
	static uint8_t encoder1 = 0x00;
	
	encoder1 = ((encoder1 << 2) | (PINB & 0x03)) & 0x0F;
	
	switch(encoder1){
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			encoder1_movement--;	//Counter Clockwise
			break;
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			encoder1_movement++;	//Clockwise
			break;
	}

}