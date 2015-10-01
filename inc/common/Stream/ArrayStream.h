#ifndef ARRAY_STREAM_H
#define ARRAY_STREAM_H

#include "Stream.h"
#include <stdlib.h>
#include <stdint.h>

namespace digitalcave {
	class ArrayStream {
		
		
		private:
			uint8_t size;
			volatile uint8_t* data;
			volatile uint8_t head;
			volatile uint8_t tail;
		public:
			ArrayStream(uint8_t size);
			uint8_t read(uint8_t *b);
			void write(uint8_t b);

			uint8_t isEmpty();
			uint8_t isFull();
	};
}

#endif

