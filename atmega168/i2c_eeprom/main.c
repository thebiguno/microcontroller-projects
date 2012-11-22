/*
 * Test program for i2c.  This particular test is made for the A T24C64C eeprom, although it 
 * can be easily modified for any other device by changing the address and the packet sequence.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/i2c/i2c_master.h"

#define ADDRESS 0x50

uint8_t message[10];
char temp[32];

int main (void){
	//Do setup here
	serial_init(9600, 8, 0, 1);
	i2c_master_init(100);

	//Enable memory in normal mode
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x01;
	message[2] = 0x01;
	message[3] = 0x42;
	message[4] = 0x41;
	message[5] = 0x40;
	i2c_start_transceiver_with_data(message, 6);

	_delay_ms(100);
	
	//Main program loop
	while (1){
		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x01; //Reset register pointer
		message[2] = 0x01; 
		i2c_start_transceiver_with_data(message, 3);

	
		message[0] = ADDRESS << 1 | I2C_READ;
		i2c_start_transceiver_with_data(message, 4);
		i2c_get_data_from_transceiver(message, 4);

		serial_write_s("value=");
		serial_write_s(itoa(message[1], temp, 16));
		serial_write_s(", ");
		serial_write_s(itoa(message[2], temp, 16));
		serial_write_s(", ");
		serial_write_s(itoa(message[3], temp, 16));
		serial_write_s("\n\r");

		_delay_ms(500);
	}
}
