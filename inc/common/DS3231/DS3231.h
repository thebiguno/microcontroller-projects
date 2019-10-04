#ifndef DS3231_h
#define DS3231_h

#include <stdlib.h>

#include <time/time.h>
#include <I2C.h>

#define DS3231_ADDRESS			0x68

namespace digitalcave {

	class DS3231 {
		public:
			//Inits the HMC5883L control object and sends the power up commands to the chip.
			DS3231(I2C* i2c);

			dc_time_t getTime();

			void setTime(dc_time_t time);
		private:
			I2C* i2c;
	};
}
#endif
