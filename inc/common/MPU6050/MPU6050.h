#ifndef MPU6050_h
#define MPU6050_h

#include <stdlib.h>
#include <math.h>
#include <dctypes.h>
#include <util/delay.h>
#include <I2C.h>

namespace digitalcave {

	class MPU6050{
		public:
			//Inits the MPU6050 control object and sends the power up commands to the chip.
			MPU6050(I2C* i2c);

			//You should only call this when the chip is on a flat, nonmoving surface.  We find the average error
			// and set the calibration variables accordingly.  You can read the calibration via get*Calib(), and persist
			// to EEPROM if desired.
			void calibrate();

			//Returns a vector of acceleration values (in g)
			vector_t getAccel();

			//Returns a vector of gyroscope values (in rad / s)
			vector_t getGyro();

			//Returns the temperature (in C)
			double getTemperature();

			//Get / set calibration data.  Order is Accel X, Y, Z, Gyro X, Y, Z, sent as an int16_t array.
			// These functions can be used to persist to / from EEPROM from main program.
			int16_t* getCalibration() { return calibration; }
			void setCalibration(int16_t* calibration);

		private:
			I2C* i2c;
			int16_t calibration[6];
	};

	#define MPU6050_ADDRESS				0x68

	// Blatently stolen from http://playground.arduino.cc/Main/MPU-6050
	//
	// Register names according to the datasheet.
	// According to the InvenSense document
	// "MPU-6000 and MPU-6050 Register Map
	// and Descriptions Revision 3.2", there are no registers
	// at 0x02 ... 0x18, but according other information
	// the registers in that unknown area are for gain
	// and offsets.
	#define MPU6050_AUX_VDDIO          0x01   // R/W
	#define MPU6050_SMPLRT_DIV         0x19   // R/W
	#define MPU6050_CONFIG             0x1A   // R/W
	#define MPU6050_GYRO_CONFIG        0x1B   // R/W
	#define MPU6050_ACCEL_CONFIG       0x1C   // R/W
	#define MPU6050_FF_THR             0x1D   // R/W
	#define MPU6050_FF_DUR             0x1E   // R/W
	#define MPU6050_MOT_THR            0x1F   // R/W
	#define MPU6050_MOT_DUR            0x20   // R/W
	#define MPU6050_ZRMOT_THR          0x21   // R/W
	#define MPU6050_ZRMOT_DUR          0x22   // R/W
	#define MPU6050_FIFO_EN            0x23   // R/W
	#define MPU6050_I2C_MST_CTRL       0x24   // R/W
	#define MPU6050_I2C_SLV0_ADDR      0x25   // R/W
	#define MPU6050_I2C_SLV0_REG       0x26   // R/W
	#define MPU6050_I2C_SLV0_CTRL      0x27   // R/W
	#define MPU6050_I2C_SLV1_ADDR      0x28   // R/W
	#define MPU6050_I2C_SLV1_REG       0x29   // R/W
	#define MPU6050_I2C_SLV1_CTRL      0x2A   // R/W
	#define MPU6050_I2C_SLV2_ADDR      0x2B   // R/W
	#define MPU6050_I2C_SLV2_REG       0x2C   // R/W
	#define MPU6050_I2C_SLV2_CTRL      0x2D   // R/W
	#define MPU6050_I2C_SLV3_ADDR      0x2E   // R/W
	#define MPU6050_I2C_SLV3_REG       0x2F   // R/W
	#define MPU6050_I2C_SLV3_CTRL      0x30   // R/W
	#define MPU6050_I2C_SLV4_ADDR      0x31   // R/W
	#define MPU6050_I2C_SLV4_REG       0x32   // R/W
	#define MPU6050_I2C_SLV4_DO        0x33   // R/W
	#define MPU6050_I2C_SLV4_CTRL      0x34   // R/W
	#define MPU6050_I2C_SLV4_DI        0x35   // R
	#define MPU6050_I2C_MST_STATUS     0x36   // R
	#define MPU6050_INT_PIN_CFG        0x37   // R/W
	#define MPU6050_INT_ENABLE         0x38   // R/W
	#define MPU6050_INT_STATUS         0x3A   // R
	#define MPU6050_ACCEL_XOUT_H       0x3B   // R
	#define MPU6050_ACCEL_XOUT_L       0x3C   // R
	#define MPU6050_ACCEL_YOUT_H       0x3D   // R
	#define MPU6050_ACCEL_YOUT_L       0x3E   // R
	#define MPU6050_ACCEL_ZOUT_H       0x3F   // R
	#define MPU6050_ACCEL_ZOUT_L       0x40   // R
	#define MPU6050_TEMP_OUT_H         0x41   // R
	#define MPU6050_TEMP_OUT_L         0x42   // R
	#define MPU6050_GYRO_XOUT_H        0x43   // R
	#define MPU6050_GYRO_XOUT_L        0x44   // R
	#define MPU6050_GYRO_YOUT_H        0x45   // R
	#define MPU6050_GYRO_YOUT_L        0x46   // R
	#define MPU6050_GYRO_ZOUT_H        0x47   // R
	#define MPU6050_GYRO_ZOUT_L        0x48   // R
	#define MPU6050_EXT_SENS_DATA_00   0x49   // R
	#define MPU6050_EXT_SENS_DATA_01   0x4A   // R
	#define MPU6050_EXT_SENS_DATA_02   0x4B   // R
	#define MPU6050_EXT_SENS_DATA_03   0x4C   // R
	#define MPU6050_EXT_SENS_DATA_04   0x4D   // R
	#define MPU6050_EXT_SENS_DATA_05   0x4E   // R
	#define MPU6050_EXT_SENS_DATA_06   0x4F   // R
	#define MPU6050_EXT_SENS_DATA_07   0x50   // R
	#define MPU6050_EXT_SENS_DATA_08   0x51   // R
	#define MPU6050_EXT_SENS_DATA_09   0x52   // R
	#define MPU6050_EXT_SENS_DATA_10   0x53   // R
	#define MPU6050_EXT_SENS_DATA_11   0x54   // R
	#define MPU6050_EXT_SENS_DATA_12   0x55   // R
	#define MPU6050_EXT_SENS_DATA_13   0x56   // R
	#define MPU6050_EXT_SENS_DATA_14   0x57   // R
	#define MPU6050_EXT_SENS_DATA_15   0x58   // R
	#define MPU6050_EXT_SENS_DATA_16   0x59   // R
	#define MPU6050_EXT_SENS_DATA_17   0x5A   // R
	#define MPU6050_EXT_SENS_DATA_18   0x5B   // R
	#define MPU6050_EXT_SENS_DATA_19   0x5C   // R
	#define MPU6050_EXT_SENS_DATA_20   0x5D   // R
	#define MPU6050_EXT_SENS_DATA_21   0x5E   // R
	#define MPU6050_EXT_SENS_DATA_22   0x5F   // R
	#define MPU6050_EXT_SENS_DATA_23   0x60   // R
	#define MPU6050_MOT_DETECT_STATUS  0x61   // R
	#define MPU6050_I2C_SLV0_DO        0x63   // R/W
	#define MPU6050_I2C_SLV1_DO        0x64   // R/W
	#define MPU6050_I2C_SLV2_DO        0x65   // R/W
	#define MPU6050_I2C_SLV3_DO        0x66   // R/W
	#define MPU6050_I2C_MST_DELAY_CTRL 0x67   // R/W
	#define MPU6050_SIGNAL_PATH_RESET  0x68   // R/W
	#define MPU6050_MOT_DETECT_CTRL    0x69   // R/W
	#define MPU6050_USER_CTRL          0x6A   // R/W
	#define MPU6050_PWR_MGMT_1         0x6B   // R/W
	#define MPU6050_PWR_MGMT_2         0x6C   // R/W
	#define MPU6050_FIFO_COUNTH        0x72   // R/W
	#define MPU6050_FIFO_COUNTL        0x73   // R/W
	#define MPU6050_FIFO_R_W           0x74   // R/W
	#define MPU6050_WHO_AM_I           0x75   // R

}
#endif
