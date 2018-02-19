/*
 * Const Stream implementation; wraps an const uint8_t*.
 * Useful on platforms like STM32 where a const array remains in flash rather than sram.
 * Don't use this on an AVR where you need to use PROGMEM instead.
 */

#ifndef NULL_STREAM_H
#define NULL_STREAM_H

#include <Stream.h>

namespace digitalcave {

	class ConstStream : public Stream {
		private:

			const uint8_t* data;
			const uint16_t len;
			uint16_t pos;

		public:
			//Initialize specifying baud rate and all other optional parameters
			ConstStream(const uint8_t*, const len);

			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			uint8_t reset();

			// Allow other overloaded functions from superclass to show up in subclass.
			using Stream::skip;
			using Stream::read;
			using Stream::write;

	};
}

#endif
