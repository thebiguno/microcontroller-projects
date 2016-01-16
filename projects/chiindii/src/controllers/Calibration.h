#ifndef CALIBRATION_h
#define CALIBRATION_h

#include <stdint.h>
#include <FramedSerialProtocol.h>

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

#define EEPROM_OFFSET	0

namespace digitalcave {
	class Status {
		private:
			PID *rate_x;
			PID *rate_y;
			PID *rate_z;
			PID *rate_x;
			PID *rate_y;
			Complementary *c;
			
		public:
			Calibration();

			void dispatch(FramedSerialMessage* message);
	};
}
#endif
