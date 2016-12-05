#ifndef MS5611_h
#define MS5611_h

#include <stdlib.h>
#include <math.h>
#include <dctypes.h>
#include <dcutil/delay.h>
#include <I2C.h>

#define MS5611_ADDRESS					(0x77)

#define MS5611_OVERSAMPLE_ULTRA_LOW_POWER		(0x00)
#define MS5611_OVERSAMPLE_LOW_POWER				(0x02)
#define MS5611_OVERSAMPLE_STANDARD				(0x04)
#define MS5611_OVERSAMPLE_HIGH_RES				(0x06)
#define MS5611_OVERSAMPLE_ULTRA_HIGH_RES		(0x08)

#define MS5611_CMD_ADC_READ				(0x00)
#define MS5611_CMD_RESET				(0x1E)
#define MS5611_CMD_CONV_D1				(0x40)
#define MS5611_CMD_CONV_D2				(0x50)
#define MS5611_CMD_READ_PROM			(0xA0)

namespace digitalcave {

	class MS5611 {
		public:
			//Inits the MS5611 control object and sends the power up commands to the chip.
			MS5611(I2C* i2c, uint8_t oversampling = MS5611_OVERSAMPLE_STANDARD);

			//You should only call this when the chip is on a flat, nonmoving surface.  We find the average error
			// and set the calibration variables accordingly.  You can read the calibration via get*Calib(), and persist
			// to EEPROM if desired.
			// void calibrate();

			//Return the raw values d1 and d2 directly from the device.  No calibration or scaling is applied.
			// Probably useful for debugging rather than real world data.
			// Pass in a uint8_t[6] array to return the data
			void getRaw(uint8_t* raw);

			//Returns the pressure value in 0.01 mbar (divide by 100 to get mbar)
			int32_t getPressure();
			int32_t getPressureFromRaw(uint8_t* raw);

			//Returns the temperature in 0.01 C (divide by 100 to get C)
			int32_t getTemperature();
			int32_t getTemperatureFromRaw(uint8_t* raw);

		private:
			I2C* i2c;
			uint8_t oversampling;
			uint8_t conversionDelay;
			uint16_t coefficient[8];
	};
}
#endif
