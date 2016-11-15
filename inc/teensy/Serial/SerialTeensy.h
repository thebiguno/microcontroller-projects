/*
 * Teensy HAL wrapper around Teensy serial library.
 */

#ifndef SERIAL_TEENSY_H
#define SERIAL_TEENSY_H

#include <Stream.h>
#include <Arduino.h>

namespace digitalcave {

	class SerialTeensy : public Stream {
		private:
			uint8_t serialPort;
			uint8_t incomingByte;
			
		public:
			//Initialize specifying baud rate and serial port
			SerialTeensy(uint32_t baud, uint8_t serialPort);
			
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