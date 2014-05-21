/*
 * Test program for i2c.  This particular test is made for the MMA7660FC accelerometer, although it 
 * can be easily modified for any other device by changing the address and the packet sequence.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/i2c/i2c_master.h"

#define ADDRESS 0x51

uint8_t message[10];
char temp[32];

int main (void){
	//Do setup here
	serial_init_b(9600);
	serial_write_s("Initializing i2c...");
	i2c_master_init(100);
	serial_write_s("Done\n\r");

	//Set date / time
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x02; //Reset register pointer to 0x02
	message[2] = 0x00;	//0 seconds
	message[3] = 0x51;	//51 minutes
	message[4] = 0x18;	//18 hours
	message[5] = 0x07;	//12th
	message[6] = 0x04;	//Thursday
	message[7] = 0x01;	//January
	message[8] = 0x12;  //2012
	serial_write_s("Setting time...");
	i2c_start_transceiver_with_data(message, 9);
	serial_write_s("Done\n\r");
	
	//Main program loop
	while (1){
		serial_write_s("Setting register address...");
		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x02; //Reset register pointer to 0x02
		i2c_start_transceiver_with_data(message, 2);
		serial_write_s(" Done\n\r");

		serial_write_s("Reading data...");
		message[0] = ADDRESS << 1 | I2C_READ;
		i2c_start_transceiver_with_data(message, 8);
		i2c_get_data_from_transceiver(message, 8);
		serial_write_s("Done\n\r");

		serial_write_s("Date is 20");
		serial_write_s(itoa(message[7], temp, 16));
		serial_write_s("-");
		serial_write_s(itoa(message[6], temp, 16));
		serial_write_s("-");
		serial_write_s(itoa(message[4], temp, 16));
		serial_write_s(" ");
		serial_write_s(itoa(message[3], temp, 16));
		serial_write_s(":");
		serial_write_s(itoa(message[2], temp, 16));
		serial_write_s(":");
		serial_write_s(itoa(message[1], temp, 16));
		serial_write_s("\n\r");

		_delay_ms(1000);
	}
}
