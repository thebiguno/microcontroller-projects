#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdint.h>

namespace digitalcave {
	class Serial {
		private:

		public:
			//Initialize specifying baud rate and defaulting other parameters
			Serial(uint32_t baud);
			//Initialize specifying baud rate and all other optional parameters
			Serial(uint32_t baud, uint8_t data, uint8_t parity, uint8_t stopBits);

			/*
			 * Reads a single byte from the serial port.  Pass in a pointer to a byte, and
			 * the function will write a single byte to that pointer.  If the read was successful,
			 * return 1; otherwise return 0.  Implementations MUST NOT block until a byte is received.
			 */
			virtual uint8_t read(uint8_t *b);

			/*
			 * Reads data into buffer of (at most) the given length - 1.  Returns the number of bytes
			 * which were read.  Implementations MUST NOT block until the entire buffer is filled.
			 * The character after the last read character will be null terminated (which is why
			 * the most you can read is length - 1).
			 */
			uint8_t read(uint8_t *a, uint8_t len);

			/*
			 * Writes a single byte to the serial port.  Implementations MUST block until the 
			 * write is completed.
			 */
			virtual void write(uint8_t data);

			/*
			 * Writes a null terminated string to the serial port.  Uses write(char) to actually send
			 * bytes to the serial port.
			 */
			void write(char *data);

			/*
			 * Writes a byte array to the serial port.  Uses write(uint8_t) to actually send bytes
			 * to the serial port.
			 */
			void write(uint8_t *data, uint8_t len);
	};
}

#endif

