#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdint.h>
#include <avr/eeprom.h>

#include <FramedSerialProtocol.h>

//Universal Controller messages are in 0x3X space...
#define MESSAGE_SAVE_CALIBRATION 0x30
#define MESSAGE_LOAD_CALIBRATION 0x31
#define MESSAGE_REQUEST_CALIBRATION_RATE_PID 0x33
#define MESSAGE_SEND_CALIBRATION_RATE_PID 0x34
#define MESSAGE_REQUEST_CALIBRATION_ANGLE_PID 0x35
#define MESSAGE_SEND_CALIBRATION_ANGLE_PID 0x36
#define MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY 0x37
#define MESSAGE_SEND_CALIBRATION_COMPLEMENTARY 0x38
#define MESSAGE_START_CALIBRATION_COMPLEMENTARY 0x39
#define MESSAGE_SEND_TUNING_DATA 0x3A
#define MESSAGE_START_MPU_CALIBRATION 0x3B


#define EEPROM_MAGIC	0x00
#define EEPROM_OFFSET	0x10

namespace digitalcave {
	class Chiindii; // forward declaration

	class Calibration {
		private:
			Chiindii *chiindii;
			
		public:
			Calibration(Chiindii *chiindii);

			void dispatch(FramedSerialMessage* message);
			void read();
			void write();
	};
}
#endif
