/*
 * Test program for i2c.  This particular test is made for the MMA7660FC accelerometer, although it 
 * can be easily modified for any other device by changing the address and the packet sequence.
 */

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include "lib/TwoWire/TwoWire.h"

int main (void){
	//Do setup here
	Wire.begin(42);
	
	uint8_t data;

	//Main program loop
	while (1){
		while (Wire.available() > 0){
			data = Wire.receive();
		}
	}
}
