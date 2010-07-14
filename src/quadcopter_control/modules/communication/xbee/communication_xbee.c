/*
 * XBee implementation of communication module.
 * 
 * Written by Wyatt Olson
 */
 
#include <avr/io.h>
#include "../../../lib/serial/serial.h"
#include "../communication.h"

union udouble {
	double d;
	uint32_t u;
};

void init_communication(){
	serial_init(9600, 8, 0, 1);
}

void _double_to_bytes(double value, uint8_t *buffer){
	union udouble converter;
	converter.d = value;
	
	buffer[3] = (converter.u >> 24) & 0xFF;
	buffer[2] = (converter.u >> 16) & 0xFF;
	buffer[1] = (converter.u >> 8) & 0xFF;
	buffer[0] = converter.u & 0xFF;
}

void communication_send(control_t control, uint8_t mode){
	uint8_t result[4];
	_double_to_bytes(control.pitch, result);
	
	serial_write_c(result[3]);
	serial_write_c(result[2]);
	serial_write_c(result[1]);
	serial_write_c(result[0]);
	
	serial_write_s("\n\rTest\n\r");
}

uint16_t communication_receive(){

	return 0x00;
}
