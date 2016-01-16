#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdint.h>
#include <avr/eeprom.h>

#include <FramedSerialProtocol.h>
#include <SerialAVR.h>
#include <PID.h>

#include "../Complementary.h"
#include "../lib/Mpu6050/Mpu6050.h"
#include "../lib/timer/timer.h"

//Universal Controller messages are in 0x3X space...
#define MESSAGE_SAVE_CALIBRATION 0x30
#define MESSAGE_LOAD_CALIBRATION 0x31
#define MESSAGE_RESET_CALIBRATION 0x32
#define MESSAGE_REQUEST_CALIBRATION_RATE_PID 0x33
#define MESSAGE_SEND_CALIBRATION_RATE_PID 0x34
#define MESSAGE_REQUEST_CALIBRATION_ANGLE_PID 0x35
#define MESSAGE_SEND_CALIBRATION_ANGLE_PID 0x36
#define MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY 0x37
#define MESSAGE_SEND_CALIBRATION_COMPLEMENTARY 0x38
#define MESSAGE_START_CALIBRATION_COMPLEMENTARY 0x39
#define MESSAGE_SEND_TUNING_DATA 0x3A

#define EEPROM_OFFSET	0

namespace digitalcave {
	class Calibration {
		private:
			PID *rate_x;
			PID *rate_y;
			PID *rate_z;
			PID *angle_x;
			PID *angle_y;
			Complementary *c_x;
			Complementary *c_y;
			
		public:
			Calibration(PID *rate_x, PID *rate_y, PID *rate_z, PID *angle_x, PID *angle_y, Complementary *c_x, Complementary *c_y);

			void dispatch(FramedSerialProtocol* protocol, Serial* serial, FramedSerialMessage* message, Mpu6050* mpu6050);
			void read();
			void write();
	};
}
#endif
