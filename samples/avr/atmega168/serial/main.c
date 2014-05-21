/*
 * Sample skeleton source file.
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

#define BAUD 57600

int main (void){
	serial_init_b(BAUD);
		
	//Main program loop
	while (1){
		//serial_write_c(serial_read_c());
		for (uint8_t i = 0; i <= 0xF; i++){
			serial_write_c(i);
//			_delay_ms(1);
		}
		_delay_ms(10);
		
	}
}
