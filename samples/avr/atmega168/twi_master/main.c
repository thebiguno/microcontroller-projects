/*
 * Test program for twi library.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/twi/twi.h"

#define ADDRESS 0x42

uint8_t message[512];
char temp[32];

int main (void){
	//Do setup here
	serial_init(9600, 8, 0, 1);
	DDRB = 0xFF;
	twi_init();
	
	for (uint16_t i = 0; i < 512; i++){
		message[i] = i;
	}

	//Main program loop
	while (1){
		twi_write_to(ADDRESS, message, 128, TWI_BLOCK, TWI_STOP);

		_delay_ms(500);
	}

}
