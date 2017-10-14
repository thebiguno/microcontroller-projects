/*
 * STM32 HAL implementation of I2C library.
 */

#ifndef I2C_HAL_H
#define I2C_HAL_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include <I2C.h>

namespace digitalcave {

	class I2CHAL : public I2C {
		public:
			//Initialize specifying baud rate and all other optional parameters
			I2CHAL(I2C_HandleTypeDef* hi2c);

			// Implementation of virtual functions declared in superclass
			void read(uint8_t address, I2CMessage* m) { HAL_I2C_Master_Receive(hi2c, address << 1, m->getData(), m->getLength(), 10); }
			void write(uint8_t address, I2CMessage* m) { HAL_I2C_Master_Transmit(hi2c, address << 1, m->getData(), m->getLength(), 10); }

			using I2C::read; // Allow other overloaded functions from superclass to show up in subclass.
			using I2C::write; // Allow other overloaded functions from superclass to show up in subclass.

		private:
			I2C_HandleTypeDef* hi2c;
	};
}

#endif
