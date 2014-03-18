#ifndef SERIAL_H
#define SERIAL_H

#define SERIAL_INTERRUPT_RX		1
#define SERIAL_INTERRUPT_TX		2

#include <avr/io.h>
#include "../ringcpp/ring.h"

class Serial {

	private:
		volatile uint8_t* regBaudLow;
		volatile uint8_t* regBaudHigh;
		volatile uint8_t* regControlStatusA;
		volatile uint8_t* regControlStatusB;
		volatile uint8_t* regControlStatusC;
		volatile uint8_t* regData;

		Ring *rx;
		Ring *tx;
		
	public:
		/*
		 * Initializes the specified USART with the given baud rate, and 8 data bits, even parity,
		 * and 2 stop bits.
		 */
		Serial(uint32_t baud, uint8_t usartNumber, uint8_t flags);

#ifndef SERIAL_DISABLE_RX
		/*
		 * Returns the total number of bytes available for reading on the rx buffer.  Does not block.
		 */
		uint8_t available();

		/*
		 * Reads a single character from the serial port, blocking until there is data to read,
		 * and sets the value at the specified pointer to the data.  For consistency with the
		 * poll() equivalent, returns 0x01.
		 */
		uint8_t take(char *c);
		
		/*
		 * Reads <length> characters from the serial port into the given character array.  This 
		 * function will block until <length> - 1 characters have been recieved.  Returns the number 
		 * of characters recieved (for consistency with the poll() equivalent of this function).
		 */
		uint8_t take(char *s, uint8_t length);
		
		/*
		 * Reads a single character from the serial port, and sets the value at the specified 
		 * pointer to the data.  If there is no data available in the rx buffer, no data is
		 * changed.  Returns 0x01 if a character was read, or 0x00 if there is nothing to 
		 * read.  Does not block.
		 */
		uint8_t poll(char *c);
		
		/*
		 * Reads any and all available characters from the serial port into the given character 
		 * array, up to a maximum of <length> - 1 characters.  Returns the actual number of 
		 * characters read.  Does not block.
		 */
		uint8_t poll(char *s, uint8_t length);
#endif

#ifndef SERIAL_DISABLE_TX
		/*
		 * Writes a single character to the tx buffer, and returns immediately without blocking.  
		 * If there is no room in the tx buffer, the data will be discarded.
		 */
		void offer(char c);
		
		/*
		 * Writes a null terminated string to the tx buffer, and returns immediately without blocking.  
		 * If there is no room in the tx buffer, the data will be discarded.
		 */
		void offer(char *s);

		/*
		 * Writes a single character to the tx buffer, and blocks until the entire tx buffer is empty.
		 * If there is no room in the tx buffer, the data will be discarded.
		 */
		void put(char c);

		/*
		 * Writes a null terminated string to the tx buffer, and blocks until the entire tx buffer is empty.
		 * If there is no room in the tx buffer, the data will be discarded.
		 */
		void put(char *s);
#endif
};
#endif
