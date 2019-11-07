#ifndef RDA5807_h
#define RDA5807_h

#include <stdlib.h>
#include <dcutil/delay.h>
#include <I2C.h>

#define RDA5807_ADDRESS			0x11

#define MIN_STATION				870
#define MAX_STATION				1080

namespace digitalcave {

	class RDA5807 {
		private:
			I2C* i2c;

			//Returns the 16 bit value at the specified register number from the register mapping.  Returns 0x0000 if the register number is invalid.
			uint16_t getRegister(uint8_t registerNumber);

			//Write the specified register with the specified value
			void setRegister(uint8_t registerNumber, uint16_t value);

		public:
			//Inits the RDA5807 control object and sends the power up commands to the chip.
			RDA5807(I2C* i2c);

			//Gets the current station.  Station is specified in 100kHz; e.g. 102.1 is shown as 1021.  Returns 0 if scanning / not set.
			uint16_t getStation();

			//Tune to the specified station.  Station is specified in 100kHz; e.g. 102.1 is passed as 1021.
			void setStation(uint16_t station);

			//Starts scanning.  Poll getStation() to get results.
			void doScan();

			//Returns the current volume, from 0x00 to 0x0F.
			uint8_t getVolume();

			//Set the volume, from 0x00 to 0x0F.
			void setVolume(uint8_t volume);
	};
}
#endif
