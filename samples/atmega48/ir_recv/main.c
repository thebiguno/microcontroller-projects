#include <avr/io.h>

#include "lib/remote/remote.h"
#include "lib/serial/serial.h"

int main (void){
	serial_init_b(9600);
	remote_init();
	
	uint8_t command;
	
	//Main program loop
	while (1){
		command = remote_get();
		if (command > 0) {
			serial_write_b(command);
		}
	}
}

