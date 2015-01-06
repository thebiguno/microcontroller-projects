/*
 * Test program for twi library.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/twi/twi.h"

#define ADDRESS 0x42

uint8_t message[6];
char temp[32];

void on_receive_service(uint8_t* data, uint16_t length){
	for (uint8_t i = 0; i < length; i++){
		serial_write_s(itoa(data[i], temp, 10));
		serial_write_c(' ');
	}
	serial_write_s("\n\r");
}

int main (void){
	//Do setup here
	serial_init(9600, 8, 0, 1);
	twi_init();
	twi_set_slave_address(ADDRESS);
	twi_attach_slave_rx_event(on_receive_service);
	

	//Main program loop
	while (1){
	}

}
