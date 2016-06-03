/*
 * STM32 HAL implementation of I2C library.
 */

#ifndef I2C_HAL_H
#define I2C_HAL_H

#include "stm32f4xx_hal.h"

#include <I2C.h>

namespace digitalcave {

	class I2CHAL : public I2C {
		public:
			//Initialize specifying baud rate and all other optional parameters
			I2CHAL(UART_HandleTypeDef* huart, uint8_t bufferSize);

			// Implementation of virtual functions declared in superclass
			void read(uint8_t address, I2CMessage* m, uint8_t length, uint8_t i2cStop);
			void write(uint8_t address, I2CMessage* m, uint8_t i2cBlock, uint8_t i2cStop);

			using I2C::read; // Allow other overloaded functions from superclass to show up in subclass.
			using I2C::write; // Allow other overloaded functions from superclass to show up in subclass.

		private:

	};
}

#endif
