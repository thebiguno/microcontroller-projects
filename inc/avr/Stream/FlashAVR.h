#ifndef FLASH_AVR_H
#define FLASH_AVR_H

#include <Stream.h>

namespace digitalcave {
	class FlashAVR : public Stream {

		private:
			uint16_t origin;
			uint16_t position;
		public:
			FlashAVR(uint16_t position);
			~FlashAVR();

			uint8_t read(uint8_t *b);
			uint16_t skip(uint16_t n);
			uint8_t write(uint8_t b);
			void reset();
			using Stream::read;
	};
}

#endif
