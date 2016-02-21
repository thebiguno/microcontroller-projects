#ifndef CHIINDII_H
#define CHIINDII_H

#include <util/delay.h>
#include <avr/io.h>

#include <dctypes.h>
#include <SerialAVR.h>

#include "lib/Mpu6050/Mpu6050.h"

#define MOTOR1_PORT		PORTE
#define MOTOR1_PIN		PORTE6
#define MOTOR2_PORT		PORTC
#define MOTOR2_PIN		PORTC7
#define MOTOR3_PORT		PORTD
#define MOTOR3_PIN		PORTD5
#define MOTOR4_PORT		PORTB
#define MOTOR4_PIN		PORTB7

#define DEGREES_TO_RADIANS(degrees) ((degrees) / 180.0 * M_PI)
#define RADIANS_TO_DEGREES(radians) ((radians) * (180.0 / M_PI))

namespace digitalcave {
	class MPUTest {
			
		private:
			Mpu6050 mpu6050;
			
		public:
			MPUTest();

			void run();
			
			void sendDebug(char* message);
			void sendDebug(const char* message);
	};
}
#endif
