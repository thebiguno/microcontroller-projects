/*
 * AVR implementation of Serial library.
 */

#ifndef SERIAL_AVR_H
#define SERIAL_AVR_H

#include <avr/io.h>

#include <Serial.h>
#include <ArrayStream.h>

namespace digitalcave {

	class Serial_AVR : public Serial {
		private:
			ArrayStream rxBuffer;
			
			//Register variables, to allow different serial port instances to point at different hardware ports
			volatile uint8_t* UBRRH;
			volatile uint8_t* UBRRL;
			volatile uint8_t* UCSRA;
			volatile uint8_t* UCSRB;
			volatile uint8_t* UCSRC;
			volatile uint8_t* UDR;
			
			uint8_t UDRE;
			uint8_t UCSZ0;
			uint8_t UCSZ1;
			uint8_t UCSZ2;
			uint8_t RXEN;
			uint8_t TXEN;
			uint8_t RXCIE;
			uint8_t U2X;
			uint8_t MPCM;
			uint8_t UPM0;
			uint8_t USBS;

			
		public:
			//Initialize specifying baud rate and all other optional parameters
			Serial_AVR(uint32_t baud, uint8_t dataBits = 8, uint8_t parity = 0, uint8_t stopBits = 1, uint8_t serialPort = 0, uint8_t bufferSize = 64);
			
			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			
			//Pass a byte to the read buffer; to be called from an ISR.
			void handleRead(uint8_t b);
	};
}

#endif