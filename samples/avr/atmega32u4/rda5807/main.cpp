/*
 * Test program for i2c.  This particular test is made for the RDA5807 FM tuner, although it
 * can be easily modified for any other device by changing the address and the packet sequence.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include <SerialUSB.h>
#include <I2CAVR.h>
#include <RDA5807.h>

#define ADDRESS 0x10

char temp[100];

using namespace digitalcave;

SerialUSB serial;
I2C* i2c;

int main (void){
	//Do setup here
	i2c = new I2CAVR();
	RDA5807 rda5807(i2c);

	DDRB |= _BV(PORTB5);

	rda5807.setStation(969);

	//Main program loop
	uint8_t volume = 0x00;
	while (1){
		rda5807.setVolume(volume);
		volume = (volume + 1) & 0x0F;

		rda5807.readRegisters();
		serial.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "value: %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X\n\r", rda5807.getRegister(0x02), rda5807.getRegister(0x03), rda5807.getRegister(0x04), rda5807.getRegister(0x05), rda5807.getRegister(0x06), rda5807.getRegister(0x07), rda5807.getRegister(0x0A), rda5807.getRegister(0x0B), rda5807.getRegister(0x0C), rda5807.getRegister(0x0D), rda5807.getRegister(0x0E), rda5807.getRegister(0x0F)));

		PORTB ^= _BV(PORTB5);

		_delay_ms(1000);

	}

}
