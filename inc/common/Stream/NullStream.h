/*
 * Null Stream implementation; never receives anything and accepts (and discards) all writing.
 * This is /dev/null for streams.
 */

#ifndef NULL_STREAM_H
#define NULL_STREAM_H

#include <Stream.h>

namespace digitalcave {

	class NullStream : public Stream {
		private:
			
		public:
			//Initialize specifying baud rate and all other optional parameters
			NullStream();
			
			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			
			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif