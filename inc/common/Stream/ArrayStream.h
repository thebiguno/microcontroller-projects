/*
 * Stream implementation of a ring buffer.  The size is determined at instntiation,
 * and you can read / write to it just like any other stream.
 */

#ifndef ARRAY_STREAM_H
#define ARRAY_STREAM_H

#include "Stream.h"

namespace digitalcave {
	class ArrayStream : public Stream {

		private:
			uint8_t capacity;
			volatile uint8_t* data;
			volatile uint8_t head;
			volatile uint8_t tail;
		public:
			ArrayStream(uint8_t capacity);
			~ArrayStream();

			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t b);

			uint8_t peek(uint8_t *b);

			void clear();
			uint8_t remaining();
			uint8_t size();

			uint8_t isEmpty();
			uint8_t isFull();

			// Allow other overloaded functions from superclass to show up in subclass.
			using Stream::skip;

			using Stream::read;
			using Stream::write;
			using Stream::reset;
	};
}

#endif
