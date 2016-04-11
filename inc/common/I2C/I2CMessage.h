#ifndef I2C_MESSAGE_H
#define I2C_MESSAGE_H

#include <stdint.h>

namespace digitalcave {

	class I2CMessage {
		private:
			//Data
			uint8_t reg;	//I2C Register
			uint8_t* data;
			uint8_t length;

		public:
			//Create a new message
			I2CMessage(uint8_t reg, uint8_t* message, uint8_t length);
		
			uint8_t getReg();
			uint8_t getLength();
			uint8_t* getData();
	};
}

#endif