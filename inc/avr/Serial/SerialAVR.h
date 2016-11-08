/*
 * AVR implementation of Serial library.
 * 
 * ***** IMPORTANT: *****
 * You need to put an ISR into your own code somewhere to handle incoming characters and pass them to the 
 * serial object.  To keep the library code separate and clean, we can't put this into the library itself.
 *		ISR(USART1_RX_vect){
 *			//Be sure to pass your serial object instance the correct data; i.e. UDR0 for serial port 0, UDR1 
 *			// for serial port 1, etc.
 *			serial.isr();
 * 		}
 */

#ifndef SERIAL_AVR_H
#define SERIAL_AVR_H

#include <avr/interrupt.h>
#include <avr/io.h>

#include <Stream.h>
#include <ArrayStream.h>

namespace digitalcave {

	class SerialAVR : public Stream {
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
			SerialAVR(uint32_t baud, uint8_t dataBits = 8, uint8_t parity = 0, uint8_t stopBits = 1, uint8_t serialPort = 0, uint8_t bufferSize = 64);
			
			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			
			//Notify serial library that there is a byte ready for reading.  This MUST be called by the serial read ISR.
			void isr();
			
			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif