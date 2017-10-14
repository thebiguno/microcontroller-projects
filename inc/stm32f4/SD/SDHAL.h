#ifndef SD_H
#define SD_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_sd.h"

#include <Stream.h>
#include <BlockDevice.h>

namespace digitalcave {
	class SD : public BlockDevice {

		private:

			uint32_t block;
			uint16_t position;
			uint8_t status;
			SD_HandleTypeDef hsd;

			uint8_t init();
			uint8_t command(uint8_t cmd, uint32_t arg, uint32_t* resp);
		public:
			/* initialize the card for 1-bit (0) or 4-bit (1) operation */
			SD(uint8_t width);
			~SD();

			void setBlock(uint32_t address);

			uint16_t skip(uint16_t n);
			uint8_t read(uint8_t* b);
			uint16_t read(uint8_t* a, uint16_t len);
			uint8_t write(uint8_t b);

			using BlockDevice::read;
			using BlockDevice::write;
	};
}

#endif
