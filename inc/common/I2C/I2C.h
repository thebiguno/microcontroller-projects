#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "I2CMessage.h"

#define I2C_NOBLOCK	0
#define I2C_BLOCK	1
#define I2C_NOSTOP	0
#define I2C_STOP	1

namespace digitalcave {

	class I2C {
		public:
			/*
			 * Writes the specified message to the given 7 bit address.
			 */
			void write(uint8_t address, I2CMessage* m) { this->write(address, m, I2C_BLOCK, I2C_STOP); }
			virtual void write(uint8_t address, I2CMessage* m, uint8_t i2cBlock, uint8_t i2cStop);

			/*
			 * Reads into the specified message from the given 7 bit address.
			 */
			void read(uint8_t address, I2CMessage* m) { this->read(address, m, m->getLength(), I2C_STOP); }
			virtual void read(uint8_t address, I2CMessage* m, uint8_t length, uint8_t i2cStop);

		private:
			//Data
			uint8_t command;
			uint8_t* data;
			uint8_t length;
	};
}

#endif
