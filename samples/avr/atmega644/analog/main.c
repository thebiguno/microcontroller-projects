#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lib/analog/analog.h"
#include "lib/serial/serial.h"

int main (void){
	serial_init(9600, 8, 0, 1);
	
	uint8_t pins[3];
	pins[0] = 0;
	pins[1] = 1;
	pins[2] = 2;
	analog_init(pins, 3, ANALOG_AREF);
		
	char temp[32];
	uint16_t data[3];

	//Main program loop
	while (1){
		analog_read_a(data);
		sprintf(temp, "%d, %d, %d\n\r", data[0], data[1], data[2]);
		serial_write_s(temp);
		sprintf(temp, "%d\n\r", analog_read_p(0));
		serial_write_s(temp);
		_delay_ms(1000);
		
	}
}
