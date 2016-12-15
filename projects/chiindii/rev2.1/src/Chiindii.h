#ifndef CHIINDII_H
#define CHIINDII_H

#define DEBUG

//How many motors.  Check doc/motor_arrangement.txt for how the motors are arranged in various configurations.
#define MOTOR_COUNT			8

#include <dcutil/delay.h>
#include <dcutil/dcmath.h>
#include <dctypes.h>
#include <FramedSerialProtocol.h>
#include <Madgwick.h>
#include <I2CHAL.h>
#include <MPU6050.h>
#include <MS5611.h>
#include <HMC5883L.h>
#include <SerialHAL.h>
#include <PID.h>

#include "Status.h"
#include "controllers/General.h"
#include "controllers/UniversalController.h"
#include "controllers/Calibration.h"
#include "controllers/Direct.h"
#include "battery/battery.h"


#ifdef DEBUG
#include <stdio.h>
#endif

#define MODE_UNARMED		0x00
#define MODE_ARMED_ANGLE	0x01
#define MODE_ARMED_RATE		0x02
#define MODE_ARMED_THROTTLE	0x03

#define CONTROLLER_NONE			0x00
#define CONTROLLER_UC			0x01
#define CONTROLLER_DIRECT		0x02
#define CONTROLLER_CALIBRATION	0x03


namespace digitalcave {
	class Chiindii {

		public:
			Chiindii(Stream* serial, I2C* i2c);

			void run();

			vector_t* getAngleSp() { return &angle_sp; }
			vector_t* getRateSp() { return &rate_sp; }
			double getThrottle() { return this->throttle_sp; }
			void setThrottle(double throttle){
				if (throttle < 0) throttle_sp = 0;
				else if (throttle > 1) throttle_sp = 1;
				this->throttle_sp = throttle;
			}

			PID* getRateX() { return &rate_x; }
			PID* getRateY() { return &rate_y; }
			PID* getRateZ() { return &rate_z; }
			PID* getAngleX() { return &angle_x; }
			PID* getAngleY() { return &angle_y; }
			PID* getAngleZ() { return &angle_z; }
			PID* getGforce() { return &gforce; }

			Madgwick* getImu() { return &imu; }

			//Sensor Hardware
			MPU6050* getMpu6050() { return &mpu6050; }
			MS5611* getMs5611() { return &ms5611; }
			HMC5883L* getHmc5883l() { return &hmc5883l; }

			Status* getStatus();

			uint8_t getBatteryLevel() { return battery_level; }
			uint8_t getBatteryPercent() { return battery_pct(); }

			uint8_t getMode() { return mode; }
			void setMode(uint8_t mode) { this->mode = mode; }

			void saveConfig();
			void loadConfig();
			void sendDebug(char* message, uint8_t length) { FramedSerialMessage response(MESSAGE_DEBUG, (uint8_t*) message, length); sendMessage(&response); }
			void sendDebug(const char* message, uint8_t length) { sendDebug((char*) message, length); }
			void sendStatus(char* message, uint8_t length) { FramedSerialMessage response(MESSAGE_STATUS, (uint8_t*) message, length); sendMessage(&response); }
			void sendStatus(const char* message, uint8_t length) { sendStatus((char*) message, length); }

			void sendMessage(FramedSerialMessage* message) { protocol.write(serial, message); }

		private:
			Stream* serial;
			I2C* i2c;

			MPU6050 mpu6050;
			MS5611 ms5611;
			HMC5883L hmc5883l;

			FramedSerialProtocol protocol;

			uint8_t mode;
			uint8_t battery_level;
			double throttle_sp;
			vector_t angle_sp;
			vector_t rate_sp;


			PID rate_x;
			PID rate_y;
			PID rate_z;
			PID angle_x;
			PID angle_y;
			PID angle_z;
			PID gforce;

			Madgwick imu;

			General general;
			Calibration calibration;
			Direct direct;
			UniversalController universalController;

			Status status;
	};

}
#endif
