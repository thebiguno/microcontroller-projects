#ifndef FLASH_STREAM_H
#define FLASH_STREAM_H

#include <Stream.h>

namespace digitalcave {
	class FlashStream : public Stream {


		private:
			uint16_t position;
			uint16_t mark;
		public:
			FlashStream(uint16_t position);
			~FlashStream();

			uint8_t read(uint8_t *b);
			uint16_t skip(uint16_t n);
			uint8_t write(uint8_t b);
			using Stream::read;
	};
}

#endif
