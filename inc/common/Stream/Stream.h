#ifndef STREAM_H
#define STREAM_H

#include <stdlib.h>
#include <stdint.h>

namespace digitalcave {
	class Stream {
		public:
			/*
			 * Reads a single byte from the stream.  Pass in a pointer to a byte, and
			 * the function will write a single byte to that pointer.  If the read was successful,
			 * return 1; otherwise return 0.  Implementations MUST NOT block until a byte is received.
			 */
			virtual uint8_t read(uint8_t* b) = 0;

			/*
			 * Writes a single byte to the stream.  Implementations MUST block until the 
			 * write is completed.
			 * Returns 1 if the write was successful; 0 otherwise.
			 */
			virtual uint8_t write(uint8_t data) = 0;

			/*
			 * Reads data into buffer of (at most) the given length - 1.  Returns the number of bytes
			 * which were read.  Implementations MUST NOT block until the entire buffer is filled.
			 * The character after the last read character will be null terminated (which is why
			 * the most you can read is length - 1).
			 */
			uint8_t read(uint8_t* a, uint8_t len);

			/*
			 * Writes a null terminated string to the stream.  Uses write(char) to actually send
			 * bytes to the stream.
			 * Returns the number of bytes which were written successfully.
			 */
			uint8_t write(char* data);
			uint8_t write(const char* data);

			/*
			 * Writes a byte array to the stream.  Uses write(uint8_t) to actually send bytes
			 * to the stream.
			 * Returns the number of bytes which were written successfully.
			 */
			uint8_t write(uint8_t* data, uint8_t len);
			
		private:

	};
}

#endif

