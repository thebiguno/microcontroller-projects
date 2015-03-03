/*
 * Test program for i2c.  This particular test is made for the A T24C64C eeprom, although it 
 * can be easily modified for any other device by changing the address and the packet sequence.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/serial/serial.h"
#include "lib/twi/twi.h"

#define ADDRESS 0x60

uint8_t message[6];
char temp[32];

int main (void){
	//Do setup here
	twi_init();

	message[0] = 0x58;	//Single write to channel 0 (see datasheet page 41)
	message[1] = 0x0F;	//Output to max
	message[2] = 0xFF;	//Output to max
	twi_write_to(ADDRESS, message, 3, TWI_BLOCK, TWI_STOP);
	
	DDRB |= _BV(PORTB5);

	//Main program loop
	while (1){
		message[0] = 0x58;	//Single write to channel 0 (see datasheet page 41)
		message[1] = 0x0F;	//Output to max
		message[2] = 0xFF;	//Output to max
		twi_write_to(ADDRESS, message, 3, TWI_BLOCK, TWI_STOP);

		//twi_readFrom(ADDRESS, message, 3, 1);
		PORTB ^= _BV(PORTB5);

		_delay_ms(1000);
	}

}
