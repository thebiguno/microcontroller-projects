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

//	rda5807.setStation(969);

	//Main program loop
	rda5807.setStation(969);
	rda5807.setVolume(3);
	uint8_t i = 0x00;
	while (1){
		i = (i + 1) & 0x0F;
		if (i == 0x00){
//			rda5807.doScan();
//			rda5807.setMute(1);
		}
		else if (i == 7){
//			rda5807.setMute(0);
		}

		serial.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "station: %d\n\r", rda5807.getStation()));

		//rda5807.readRegisters();
		rda5807_registers_t* r = rda5807.getRaw();
		//r.data[
		//serial.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "station: %d\n\r", rda5807.getStation()));
		serial.write((uint8_t*) temp, (uint16_t) snprintf(temp, sizeof(temp), "0: %02X%02X 2: %02X%02X 3: %02X%02X 4: %02X%02X 5: %02X%02X 6: %02X%02X 7: %02X %02X \n\r", r->data[0], r->data[1], r->data[4], r->data[5], r->data[6], r->data[7], r->data[8], r->data[9], r->data[10], r->data[11], r->data[12], r->data[13], r->data[14], r->data[15]));

		PORTB ^= _BV(PORTB5);

		_delay_ms(1000);

	}

}
