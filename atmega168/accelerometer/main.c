/*
 * Sample file for MMA7455L Accelerometer (using I2C communications)
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/i2c/i2c_master.h"

#define ADDRESS 0x1D

uint8_t message[8];
uint8_t data[3];
char temp[32];

int16_t convert(int8_t raw){
//	return (raw / 64.0) * 1000;
	return raw;
}

void get_average_reading(uint8_t *data, uint8_t count){
	int16_t total[3] = {0};
	for (uint8_t i = 0; i < count; i++){
		uint8_t ready = 0x00;
		while (!ready){
			//Check if data is ready
			message[0] = ADDRESS << 1 | I2C_WRITE;
			message[1] = 0x09; //Reset register pointer to 0x09 (Status)
			i2c_start_transceiver_with_data(message, 2);

			message[0] = ADDRESS << 1 | I2C_READ;
			i2c_start_transceiver_with_data(message, 2);
			i2c_get_data_from_transceiver(message, 2);	

			ready = message[1] & 0x1;
		}

		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x06; //Reset register pointer to 0x06 (8bit X)
		i2c_start_transceiver_with_data(message, 2);

		message[0] = ADDRESS << 1 | I2C_READ;
		i2c_start_transceiver_with_data(message, 4);
		i2c_get_data_from_transceiver(message, 4);
		
		total[0] += (int8_t) message[1];
		total[1] += (int8_t) message[2];
		total[2] += (int8_t) message[3];
	}
	
	data[0] = total[0] / count;
	data[1] = total[1] / count;
	data[2] = total[2] / count;
}

void show_reading(){
	get_average_reading(data, 0x10);
		
	serial_write_s("x=");
	serial_write_s(itoa(convert(data[0]), temp, 16));
	serial_write_s(", y=");
	serial_write_s(itoa(convert(data[1]), temp, 16));
	serial_write_s(", z=");
	serial_write_s(itoa(convert(data[2]), temp, 16));
	serial_write_s("\n\r");
}

int main (void){
	//Do setup here
	serial_init(9600, 8, 0, 1);
	i2c_master_init(400);

	//Enable accelerometer in 2g measurement mode
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x16;
	message[2] = _BV(6) | _BV(2) | _BV(0); //6 == Disable external ready pin, 2 == 2g, 0 == measurement mode
	i2c_start_transceiver_with_data(message, 3);

	//Turn on 125Hz Filtering
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x18;
	message[2] = _BV(7);
	i2c_start_transceiver_with_data(message, 3);

	
	serial_write_s("milli-G Values (1000 == 1.000g)\n\r");

	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x10;
	message[2] = 0x00;
	message[3] = 0x00;
	message[4] = 0x00;
	message[5] = 0x00;
	message[6] = 0x00;
	message[7] = 0x00;
	i2c_start_transceiver_with_data(message, 8);

	show_reading();

	//Calibrate device to 0g on X / Y axis and 1g on Z
	get_average_reading(data, 0xF0);
		
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x10;
	message[2] = data[0] * -2;
	message[3] = 0x00;
	message[4] = data[1] * -2;
	message[5] = 0x00;
	message[6] = (64 - data[2]) * 2;
	message[7] = 0x00;
	i2c_start_transceiver_with_data(message, 8);
	
	
	//Main program loop
	while (1){
		show_reading();
	}
}
