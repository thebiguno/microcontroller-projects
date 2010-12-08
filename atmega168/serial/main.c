/*
 * Sample skeleton source file.
 */


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

int main (void){
	serial_init(57600, 8, 0, 1);
		
	char c;

	//Main program loop
	while (1){
		if (serial_read_c(&c)){
			serial_write_c(c);
		}		
	}
}
