#ifndef STREAM_H
#define STREAM_H

#include <stdlib.h>
#include <stdint.h>

namespace digitalcave {
	class Stream {
		public:
			virtual ~Stream() {}

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
			virtual uint8_t write(uint8_t b) = 0;

			/*
			 * Reads data into buffer of (at most) the given length - 1.  Returns the number of bytes
			 * which were read.  Implementations MUST NOT block until the entire buffer is filled.
			 * The character after the last read character will be null terminated (which is why
			 * the most you can read is length - 1).
			 */
			uint16_t read(uint8_t* a, uint16_t len);

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
			uint16_t write(uint8_t* data, uint16_t len);

			/*
			 * Flushes any buffered output.
			 */
			uint8_t flush();

			/*
			 * Skip over n bytes in the stream.
			 * The default implementation reads and discards a byte at a time but
			 * sub-clases may implement this in a more efficient manner.
			 * Returns the number of bytes successfully skipped.
			 */
			uint16_t skip(uint16_t b);

			/*
			 * Resets the stream back to it's original position.
			 * If the reset was successful return 1; otherwise return 0.
			 * The default implementation does nothing and returns 0.
			 */
			uint8_t reset();
		private:

	};
}

#endif
