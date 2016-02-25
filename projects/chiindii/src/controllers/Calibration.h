#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <stdint.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include <FramedSerialProtocol.h>

//Calibration messages are in 0x3X space...
#define MESSAGE_SAVE_CALIBRATION 0x30
#define MESSAGE_LOAD_CALIBRATION 0x31
#define MESSAGE_CALIBRATE_IMU 0x32
#define MESSAGE_MADGWICK_TUNING 0x33
#define MESSAGE_RATE_PID_TUNING 0x34
#define MESSAGE_ANGLE_PID_TUNING 0x35
#define MESSAGE_THROTTLE_PID_TUNING 0x36

#define EEPROM_MAGIC	0x2FF
#define EEPROM_OFFSET	0x300

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
