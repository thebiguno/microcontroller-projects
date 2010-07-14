/*
 * XBee implementation of communication module.
 * 
 * Written by Wyatt Olson
 */
 
#include <avr/io.h>
#include "../../../lib/serial/serial.h"
#include "../communication.h"

void init_communication(){
	serial_init(9600, 8, 0, 1);
}

void communication_send(control_t control, uint8_t mode){
	uint32_t temp = (uint32_t) control.pitch;
	
	serial_write_c((temp >> 24) & 0xFF);
	serial_write_c((temp >> 16) & 0xFF);
	serial_write_c((temp >> 8) & 0xFF);
	serial_write_c(temp & 0xFF);
	
	serial_write_s("\n\rTest\n\r");
}

uint16_t communication_receive(){

	return 0x00;
}
