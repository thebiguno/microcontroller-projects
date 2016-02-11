#ifndef STUBBY_H
#define STUBBY_H

#include "Leg.h"

#ifndef DEBUG_SIMULATION
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdint.h>
#include <string.h>

#include <FramedSerialProtocol.h>
#include <SerialAVR.h>
#include <UniversalControllerClient.h>

#include "util/delays.h"
#include "lib/pwm/pwm.h"
#else
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#endif

#include "hardware.h"

//State variables
#define POWER_OFF				0x00
#define POWER_ON				0x01

#define CONTROLLER_NONE			0x00
#define CONTROLLER_UC			0x01
#define CONTROLLER_PROCESSING	0x02
#define CONTROLLER_CALIBRATION	0x03


namespace digitalcave {
	class Stubby {
			
		private:
			Leg* legs;
			FramedSerialProtocol protocol;
			
		public:
			Stubby();
			
			FramedSerialProtocol* getProtocol();
			Leg* getLegs();
			void run();
			void sendDebug(char* message);
			void resetLegs();
	};
}

uint8_t get_power();
void set_power(uint8_t power);

uint8_t get_controller();

void doAcknowledgeCommand(uint8_t command);
void doCompleteCommand(uint8_t command);
void doSendDebug(char* message);
void doResetLegs();

void mode_select();
void mode_remote_control();
void mode_calibration();
	
#endif
