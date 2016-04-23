#ifndef STUBBY_H
#define STUBBY_H

#include "Leg.h"

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <string.h>

#include <FramedSerialProtocol.h>
#include <SerialAVR.h>
#include <UniversalControllerClient.h>

#include "gait/gait.h"
#include "hardware/magnetometer.h"
#include "lib/pwm/pwm.h"

#include "hardware.h"

//State variables
#define STATE_POWER_OFF			0x00
#define STATE_WALKING			0x01
#define STATE_CALIBRATION		0x02


namespace digitalcave {
	class Stubby {
			
		private:
			Leg* legs;
			FramedSerialProtocol protocol;
			
			uint8_t state;
			float linearAngle;
			float linearVelocity;
			float rotationalVelocity;
			float turn;
			
		public:
			Stubby();
			
			FramedSerialProtocol* getProtocol();
			Leg* getLegs();
			
			uint8_t getState() {return state;}
			void setState(uint8_t state) { this->state = state;}
			void setLinearAngle(float linearAngle) { this->linearAngle = linearAngle;}
			void setLinearVelocity(float linearVelocity) { this->linearVelocity = linearVelocity;}
			void setRotationalVelocity(float rotationalVelocity) { this->rotationalVelocity = rotationalVelocity;}
			void setTurn(int8_t turn) { this->turn = turn;}
			
			void run();
			void sendDebug(char* message);
			void resetLegs();
	};
}
	
#endif
