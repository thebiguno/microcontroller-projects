#ifndef HMC5883L_h
#define HMC5883L_h

#include <stdlib.h>
#include <math.h>
#include <dctypes.h>
#include <dcutil/delay.h>
#include <I2C.h>

#define HMC5883L_ADDRESS					(0x1E)

#define HMC5883L_OVERSAMPLE_ULTRA_LOW_POWER	(0x00)
#define HMC5883L_OVERSAMPLE_LOW_POWER		(0x02)
#define HMC5883L_OVERSAMPLE_STANDARD		(0x04)
#define HMC5883L_OVERSAMPLE_HIGH_RES		(0x06)
#define HMC5883L_OVERSAMPLE_ULTRA_HIGH_RES	(0x08)

#define HMC5883L_CONFIG_REG_A				(0x00)
#define HMC5883L_CONFIG_REG_B				(0x01)
#define HMC5883L_CONFIG_MODE				(0x02)
#define HMC5883L_CONFIG_DATA_OUTPUT_X_MSB	(0x03)
#define HMC5883L_CONFIG_DATA_OUTPUT_X_LSB	(0x04)
#define HMC5883L_CONFIG_DATA_OUTPUT_Z_MSB	(0x05)
#define HMC5883L_CONFIG_DATA_OUTPUT_Z_LSB	(0x06)
#define HMC5883L_CONFIG_DATA_OUTPUT_Y_MSB	(0x07)
#define HMC5883L_CONFIG_DATA_OUTPUT_Y_LSB	(0x08)
#define HMC5883L_CONFIG_STATUS				(0x09)
#define HMC5883L_CONFIG_ID_A				(0x0A)
#define HMC5883L_CONFIG_ID_B				(0x0B)
#define HMC5883L_CONFIG_ID_C				(0x0C)

namespace digitalcave {

	class HMC5883L {
		public:
			//Inits the HMC5883L control object and sends the power up commands to the chip.
			HMC5883L(I2C* i2c, vector_t calibration = {0, 0, 0});

			//Return the raw values in order Xmsb, Xlsb, Ymsb, Ylsb, Zmsb, Zlsb directly from the device.  No calibration or scaling is applied.
			// Probably useful for debugging rather than real world data.
			// Pass in a uint8_t[6] array to return the data
			void getRaw(uint8_t* raw);

			//Returns the magnetometer reading values.  The units will depend on the gain; default gain results in 0.92mG / LSB.
			vector_t getMag();
			vector_t getMagFromRaw(uint8_t* raw);

			//Returns a compass heading in radians, from -PI to PI.  Only uses the X and Y axis,
			// so accuracy will be reduced if the module is not flat.
			float getHeading(vector_t values);

			vector_t getCalibration() { return calibration; }
			void setCalibration(vector_t calibration) { this->calibration = calibration; }

			//Calculate calibration values.  While calibrating, you should spin the device on all axis.
			// While calibrating, the device will constantly read values over the course of 10 seconds.
			// Be sure to disable watchdog timers while calibrating.
			void calibrate();

		private:
			I2C* i2c;
			vector_t calibration;
	};
}
#endif
