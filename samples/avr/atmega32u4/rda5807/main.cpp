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
	rda5807.setStation(969);
	rda5807.setVolume(15);
//	uint8_t i = 0x00;
	while (1){
		// i = (i + 1) & 0x0F;
		// if (i == 0x00){
		// 	rda5807.doScan();
		// rda5807.setMute(1);
		// }
		// else if (i == 7){
		// rda5807.setMute(0);
		// }


		serial.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "station: %d, signal: %d\n\r", rda5807.getStation(), rda5807.getSignalStrength()));

		PORTB ^= _BV(PORTB5);

		_delay_ms(1000);

	}

}
