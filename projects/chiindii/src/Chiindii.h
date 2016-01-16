#ifndef CHIINDII_H
#define CHIINDII_H

#include <util/delay.h>
#include <avr/io.h>

#include <FramedSerialProtocol.h>

#include "Complementary.h"
#include "Status.h"
#include "controllers/UniversalController.h"
#include "controllers/Calibration.h"
#include "controllers/Direct.h"

#define MOTOR1_PORT		PORTE
#define MOTOR1_PIN		PORTE6
#define MOTOR2_PORT		PORTC
#define MOTOR2_PIN		PORTC7
#define MOTOR3_PORT		PORTD
#define MOTOR3_PIN		PORTD5
#define MOTOR4_PORT		PORTB
#define MOTOR4_PIN		PORTB7

#define MODE_UNARMED		0x00
#define MODE_ARMED_RATE		0x01
#define MODE_ARMED_ANGLE	0x02

#define CONTROLLER_NONE			0x00
#define CONTROLLER_UC			0x01
#define CONTROLLER_DIRECT		0x02
#define CONTROLLER_CALIBRATION	0x03


namespace digitalcave {
	class Chiindii {
			
		private:
			uint8_t mode;
			double throttle;
			vector_t angle_sp;
			vector_t rate_sp;
			
			SerialAVR serial;
			FramedSerialProtocol protocol;
			FramedSerialMessage message;
			Mpu6050 mpu6050;

			PID rate_x;
			PID rate_y;
			PID rate_z;
			PID angle_x;
			PID angle_y;
			Complementary c_x;
			Complementary c_y;
		
			Direct direct;
			UniversalController uc;
			Calibration calibration;
			
			Status status;
			
			void driveMotors();
			void dispatch();
			
		public:
			Chiindii();

			void run();
			
			vector_t getAngleSp();
			vector_t getRateSP();
			
			PID getRateX();
			PID getRateY();
			PID getRateZ();
			PID getAngleX();
			PID getAngleY();
			Complementary getCompX();
			Complementary getComyY();
			
			void setMode(uint8_t mode);
			void setThrottle(double throttle);
			
			void getStatus();
	};
}
#endif
