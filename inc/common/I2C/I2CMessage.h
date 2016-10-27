#ifndef I2C_MESSAGE_H
#define I2C_MESSAGE_H

#include <stdint.h>

namespace digitalcave {

	class I2CMessage {
		private:
			//Data
			uint8_t* data;
			uint8_t length;

		public:
			//Create a new message
			I2CMessage(uint8_t* data, uint8_t length);

			uint8_t getLength() { return length; }
			uint8_t* getData() { return data; }

			//Change the length.  Do not set this to be longer than data array,
			// or you will overflow.
			void setLength(uint8_t length) { this->length = length; }
	};
}

#endif
