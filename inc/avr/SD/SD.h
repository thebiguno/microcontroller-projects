#ifndef SD_H
#define SD_H

#include <Stream.h>
#include <BlockDevice.h>

namespace digitalcave {
	class SD : public BlockDevice {

		private:
			volatile uint8_t* port_cs;
			uint8_t pin_cs;
			uint32_t block;
			uint16_t position;
			uint8_t status;

			uint8_t init();
			void fast_clock();
			void slow_clock();
			void select();
			void deselect();
			uint8_t transfer(uint8_t b);
			uint8_t command(uint8_t cmd, uint32_t arg, uint32_t* resp);
		public:
			SD(volatile uint8_t* cs_port, uint8_t cs_pin);
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
