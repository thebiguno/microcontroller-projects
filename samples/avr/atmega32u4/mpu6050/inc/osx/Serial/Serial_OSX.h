/*
 * OSX implementation of Serial library.
 */

#ifndef SERIAL_OSX_H
#define SERIAL_OSX_H

#include "../../common/Stream/Stream.h"

namespace digitalcave {

	class Serial_OSX : public Stream {
		private:
			int fd;
			
		public:
			//Initialize specifying baud rate and all other optional parameters
			Serial_OSX(char* device, uint32_t baud, uint8_t dataBits = 8, uint8_t parity = 0, uint8_t stopBits = 1);
			
			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			
			void close();
			
			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif