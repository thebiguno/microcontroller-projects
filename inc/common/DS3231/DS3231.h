#ifndef DS3231_h
#define DS3231_h

#include <stdlib.h>
#include <math.h>
#include <dctypes.h>
#include <dcutil/delay.h>
#include <I2C.h>

#define DS3231_ADDRESS			0x68

#define DS3231_MODE_AM			0
#define DS3231_MODE_PM			1
#define DS3231_MODE_24			2

typedef struct ds3231_time {
	uint8_t year;
	uint8_t month;
	uint8_t day_of_month;
	uint8_t day_of_week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t mode;	//DS3231_MODE_AM (0) = AM, DS3231_MODE_PM (1) = PM, DS3231_MODE_24 (2) = 24 hour
} ds3231_time_t;

namespace digitalcave {

	class DS3231 {
		public:
			//Inits the HMC5883L control object and sends the power up commands to the chip.
			DS3231(I2C* i2c);

			ds3231_time_t getTime();

			void setTime(ds3231_time_t time);
		private:
			I2C* i2c;
	};
}
#endif
