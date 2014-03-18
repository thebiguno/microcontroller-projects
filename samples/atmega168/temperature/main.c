/*
 * Returns the temperature as read from the TC74 temperature sensor.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/i2c/i2c_master.h"

#define ADDRESS 0x4D

uint8_t message[4];
char temp[32];

int main (void){
	//Do setup here
	serial_init_b(9600);
	
	serial_write_s("Statup 1...\n\r");
		
	i2c_master_init(400);

	serial_write_s("Statup 2...\n\r");

	//Main program loop
	while (1){
		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x00;
		message[2] = 0x00;
		i2c_start_transceiver_with_data(message, 3);
//		i2c_get_data_from_transceiver(message, 2);
	
		message[0] = ADDRESS << 1 | I2C_READ;
		message[1] = 0x00;
		i2c_start_transceiver_with_data(message, 3);
//		serial_write_s("reading...\n\r");
		i2c_get_data_from_transceiver(message, 3);
//		serial_write_s("done reading...\n\r");

		serial_write_s("x=");
		serial_write_s(itoa(message[0], temp, 16));
		serial_write_s(", y=");
		serial_write_s(itoa(message[1], temp, 16));
		serial_write_s(", z=");
		serial_write_s(itoa(message[2], temp, 16));
		serial_write_s("\n\r");

		_delay_ms(1000);
	}
}
