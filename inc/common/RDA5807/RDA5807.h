#ifndef RDA5807_h
#define RDA5807_h

#include <stdlib.h>
#include <dcutil/delay.h>
#include <I2C.h>

#define RDA5807_ADDRESS			0x11

#define MIN_STATION				870
#define MAX_STATION				1080

typedef union {
	struct fields {
		uint8_t r0_chipId;
		uint8_t r0_unused1;

		uint8_t r2_dhiz : 1;
		uint8_t r2_dmute : 1;
		uint8_t r2_mono : 1;
		uint8_t r2_bass : 1;
		uint8_t r2_rclk : 1;
		uint8_t r2_rclk_di : 1;
		uint8_t r2_seekup : 1;
		uint8_t r2_seek : 1;
		uint8_t r2_seek_mode : 1;
		uint8_t r2_clk_mode : 3;
		uint8_t r2_rds_en : 1;
		uint8_t r2_new_method : 1;
		uint8_t r2_soft_reset : 1;
		uint8_t r2_enable : 1;

		uint16_t r3_channel : 10;
		uint8_t r3_direct : 1;
		uint8_t r3_tune : 1;
		uint8_t r3_band : 2;
		uint8_t r3_space : 2;
		
		uint8_t r4;
		uint8_t r5;
		uint8_t r6;
		uint8_t r7;
		uint8_t rA;
		uint8_t rB;
		uint8_t rC;
		uint8_t rD;
		uint8_t rE;
		uint8_t rF;
	};
	struct registers {
		uint16_t r0;
		uint16_t r2;
		uint16_t r3;
		uint16_t r4;
		uint16_t r5;
		uint16_t r6;
		uint16_t r7;
		uint16_t rA;
		uint16_t rB;
		uint16_t rC;
		uint16_t rD;
		uint16_t rE;
		uint16_t rF;
	};
	struct rawData {
		uint8_t data[26];
	}
} rda5807_registers_t;

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
