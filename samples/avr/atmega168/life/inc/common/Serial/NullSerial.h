/*
 * Null Serial implementation; never receives anything and accepts (and discards) all writing.
 * This is /dev/null for serial ports.
 */

#ifndef NULL_SERIAL_H
#define NULL_SERIAL_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include <Serial.h>

namespace digitalcave {

	class NullSerial : public Serial {
		private:
			
		public:
			//Initialize specifying baud rate and all other optional parameters
			NullSerial();
			
			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			
			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif