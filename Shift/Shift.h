#ifndef SHIFT_H
#define SHIFT_H

#include <avr/io.h>

class Shift {

	private:
		
		volatile uint8_t *dataPort;
		volatile uint8_t *dataDDR;
		uint8_t dataPin;
		volatile uint8_t *clockPort;
		volatile uint8_t *clockDDR;
		uint8_t clockPin;
		
	public:
		
		/*
		 * Constructs the Shift interface.  You can do this multiple times for different pin combinations.
		 * This does not change DDRs; that is done at send() / receive() time.
		 */
		Shift(volatile uint8_t *dataPort, uint8_t dataPin, volatile uint8_t *clockPort, uint8_t clockPin);
	
		/*
		 * Sends the data, one byte at a time.  Blocks until the entire array is sent.
		 */
		void send(uint8_t data*, uint16_t length);

		/*
		 * Receives a data array, one byte at a time.  Blocks until the entire requested length is received.
		 */
		uint16_t receive(uint8_t data*, uint16_t length);
};
#endif
