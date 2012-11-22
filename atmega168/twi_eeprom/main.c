/*
 * Test program for i2c.  This particular test is made for the A T24C64C eeprom, although it 
 * can be easily modified for any other device by changing the address and the packet sequence.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/twi/twi.h"

#define ADDRESS 0x50

uint8_t message[6];
char temp[32];

int main (void){
	//Do setup here
	serial_init(9600, 8, 0, 1);
	DDRB = 0xFF;
	twi_init();

	message[0] = 0x01;
	message[1] = 0x01;
	message[2] = 0x42;
	message[3] = 0x41;
	message[4] = 0x40;
	twi_writeTo(ADDRESS, message, 5, 1, 1);

	//Main program loop
	while (1){
		message[0] = 0x01; //Reset register pointer
		message[1] = 0x01; 
		twi_writeTo(ADDRESS, message, 2, 1, 1);

	
		twi_readFrom(ADDRESS, message, 3, 1);

		serial_write_s("value=");
		serial_write_s(itoa(message[0], temp, 16));
		serial_write_s(", ");
		serial_write_s(itoa(message[1], temp, 16));
		serial_write_s(", ");
		serial_write_s(itoa(message[2], temp, 16));
		serial_write_s("\n\r");

		_delay_ms(500);
	}

}
