#ifndef STATUS_h
#define STATUS_h

#include <stdint.h>

namespace digitalcave {
	class Status {
		private:
			uint32_t lastTime;
			uint8_t status;

		public:
			//Constructor
			Status();

			void poll(uint32_t time);

			void batteryLow();
			void batteryOK();
			void armed();
			void disarmed();
			void commInterrupt();
			void commOK();
	};
}
#endif
