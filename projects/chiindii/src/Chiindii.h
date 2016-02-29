#ifndef CHIINDII_H
#define CHIINDII_H

#include <util/delay.h>
#include <avr/io.h>

#define DEBUG

#include <dcmath.h>
#include <dctypes.h>
#include <FramedSerialProtocol.h>
#include <Mpu6050.h>
#include <SerialAVR.h>
#include <PID.h>

#include "Status.h"
#include "controllers/General.h"
#include "controllers/UniversalController.h"
#include "controllers/Calibration.h"
#include "controllers/Direct.h"

#include "imu/Madgwick.h"

#ifdef DEBUG
#include <stdio.h>
#endif

#define MOTOR1_PORT		PORTE
#define MOTOR1_PIN		PORTE6
#define MOTOR2_PORT		PORTC
#define MOTOR2_PIN		PORTC7
#define MOTOR3_PORT		PORTD
#define MOTOR3_PIN		PORTD5
#define MOTOR4_PORT		PORTB
#define MOTOR4_PIN		PORTB7

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
			
		private:
			uint8_t mode;
			uint8_t debug;
			uint8_t battery_level;
			double throttle_sp;
			vector_t angle_sp;
			vector_t rate_sp;
			
			FramedSerialProtocol protocol;
			Mpu6050 mpu6050;

			PID rate_x;
			PID rate_y;
			PID rate_z;
			PID angle_x;
			PID angle_y;
			PID angle_z;
			PID gforce;

			Madgwick madgwick;
		
			General general;
			Calibration calibration;
			Direct direct;
			UniversalController uc;
			
			Status status;
			
		public:
			Chiindii();

			void run();
			
			vector_t* getAngleSp();
			vector_t* getRateSp();
			double getThrottle();
			void setThrottle(double throttle);

			PID* getRateX();
			PID* getRateY();
			PID* getRateZ();
			PID* getAngleX();
			PID* getAngleY();
			PID* getAngleZ();
			PID* getGforce();

			Madgwick* getMadgwick();
			Mpu6050* getMpu6050();
			Status* getStatus();

			uint8_t getBatteryLevel();
			uint8_t getBatteryPercent();
			
			uint8_t getMode();
			void setMode(uint8_t mode);
			
			void saveConfig();
			void loadConfig();
			uint8_t getDebug();
			void setDebug(uint8_t debug);
			void sendDebug(char* message, uint8_t length);
			void sendDebug(const char* message, uint8_t length);
			void sendStatus(char* message, uint8_t length);
			void sendStatus(const char* message, uint8_t length);

			void sendMessage(FramedSerialMessage* message);
	};
}
#endif
