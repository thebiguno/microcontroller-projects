/*
 * Test program for i2c.  This particular test is made for the MMA7660FC accelerometer, although it 
 * can be easily modified for any other device by changing the address and the packet sequence.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/i2c/i2c_master.h"

#define ADDRESS 0x4C

uint8_t message[4];
char temp[32];

int convert(uint8_t raw){
	int8_t data = raw << 2; //Shift two bits over
	return ((float) data * 0.047) / 4.0 * 1000;
}

int main (void){
	//Do setup here
	serial_init(9600, 8, 0, 1);
	i2c_master_init(100);

	//Enable accelerometer in normal mode
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x07;
	message[2] = 0x01;
	i2c_start_transceiver_with_data(message, 3);
	
	serial_write_s("milli-G Values (1000 == 1.000g)\n\r");
	
	//Main program loop
	while (1){
		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x00; //Reset register pointer to 0x00
		i2c_start_transceiver_with_data(message, 2);

		message[0] = ADDRESS << 1 | I2C_READ;
		i2c_start_transceiver_with_data(message, 4);
		i2c_get_data_from_transceiver(message, 4);

		serial_write_s("x=");
		serial_write_s(itoa(convert(message[1]), temp, 10));
		serial_write_s(", y=");
		serial_write_s(itoa(convert(message[2]), temp, 10));
		serial_write_s(", z=");
		serial_write_s(itoa(convert(message[3]), temp, 10));
		serial_write_s("\n\r");

		_delay_ms(50);
	}
}
