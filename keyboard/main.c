#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "lib/serial/serial.h"

int main (void){
	serial_init_b(9600);

	//Main program loop
	while (1){
		serial_write_s("Testing 1, 2, 3\n\r");
		_delay_ms(1000);
		
	}
}
