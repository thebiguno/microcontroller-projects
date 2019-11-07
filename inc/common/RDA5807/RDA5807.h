#ifndef RDA5807_h
#define RDA5807_h

#include <stdlib.h>
#include <dcutil/delay.h>
#include <I2C.h>

#define RDA5807_ADDRESS			0x10
#define RDA5807_ADDRESS_RANDOM	0x11

#define MIN_STATION				870
#define MAX_STATION				1080

namespace digitalcave {

	class RDA5807 {
		private:
			I2C* i2c;

			//Mapping of the register addresses.  Write registers start at index 0 = register 0x02 HIGH, 1 = 0x02 LOW, etc.  Read registers start at index 12 = 0x0A HIGH, etc.
			uint8_t registers[24];


		public:
			//Inits the RDA5807 control object and sends the power up commands to the chip.
			RDA5807(I2C* i2c);

			//Reads the registers from the device, and updates the registers mapping variable
			void readRegisters();
			//Writes the registers to the device.  We only write 0x02 through 0x07, as the others are read only.
			void writeRegisters();

			void writeRegister(uint8_t registerNumber);

			//Returns the 16 bit value at the specified register number from the register mapping.  Returns 0x0000 if the register number is invalid.
			uint16_t getRegister(uint8_t registerNumber);

			//Updates the 16 bit value in the register mapping at the specified register number
			void setRegister(uint8_t registerNumber, uint16_t value);

			//Gets the current station.  Returns 0 if scanning / not set.
			//uint16_t getStation();

			//Returns the current station.  Station is specified in 100kHz; e.g. 102.1 is shown as 1021.
			uint16_t getStation();

			//Tune to the specified station.  Station is specified in 100kHz; e.g. 102.1 is passed as 1021.
			void setStation(uint16_t station);

			//Starts scanning.  Poll getScanStatus() to get results.
			//void scan();

			//
			//uint8_t getScanStatus();

			//Set the volume, from 0x00 to 0x0F.  You need to call writeRegisters() to commit the change.
			void setVolume(uint8_t volume);
	};
}
#endif
