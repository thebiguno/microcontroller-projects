/*
 * Toggles pin B4
 */

#include "analog.h"
#include "usb/serial.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

int main (void){
	//Init ADC pins with reference of AVCC (with external cap at AREF).
	uint8_t analog_pins[1];
	analog_pins[0] = 0;		//F0
	analog_init(analog_pins, 1, ANALOG_AVCC);
	
	//Pullup on ADC input
	PORTF |= _BV(PORTF0);
	
	usb_init();
	
	//Main program loop
	while (1){
		char temp[32];
		uint8_t size = snprintf(temp, sizeof(temp), "ADC: %d\n", analog_read_p(0));
		usb_serial_write((const uint8_t*) temp, size);
		_delay_ms(500);
	}
}

