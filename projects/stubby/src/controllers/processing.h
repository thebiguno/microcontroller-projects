#ifndef PROCESSING_H
#define PROCESSING_H

#include <avr/wdt.h>
#include <stdint.h>
#include <stdio.h>

#include "../Stubby.h"
#include "../Leg.h"
#include "../gait/gait.h"
#include "../hardware/battery.h"
#include "../hardware/distance.h"
#include "../hardware/magnetometer.h"
#include "../hardware/status.h"
#include "../util/convert.h"

//PC / Stubby-specific messages are in 0x2X space...
#define MESSAGE_REQUEST_POWER_ON				0x20
#define MESSAGE_REQUEST_POWER_OFF				0x21
#define MESSAGE_REQUEST_MOVE					0x22
#define MESSAGE_REQUEST_TURN					0x23
#define MESSAGE_REQUEST_TRANSLATE				0x24
#define MESSAGE_REQUEST_ROTATE					0x25
#define MESSAGE_REQUEST_HEADING					0x26
#define MESSAGE_SEND_HEADING					0x27
#define MESSAGE_REQUEST_DISTANCE				0x28
#define MESSAGE_SEND_DISTANCE					0x29
#define MESSAGE_REQUEST_OPTICAL					0x2A
#define MESSAGE_SEND_OPTICAL					0x2B
#define MESSAGE_REQUEST_SET_LED					0x2C

#if MAGNETOMETER == 0
#warning   Without a magnetometer, Processing API command MESSAGE_REQUEST_TURN will not work, and MESSAGE_REQUEST_MOVE will not be accurate.
#endif

namespace digitalcave {
	class Processing {
			
		private:
			Stubby* stubby;
			
		public:
			Processing(Stubby* stubby);
			
			/*
			 * Called from the Stubby protocol dispatch function
			 */
			void dispatch(Serial* serial, FramedSerialMessage* message);
	};
}

#endif