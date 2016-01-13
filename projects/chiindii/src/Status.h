#ifndef STATUS_h
#define STATUS_h

#include <stdint.h>

using namespace digitalcave;

namespace digitalcave {
	class Status {
		private:
			uint32_t lastTime;
			uint8_t status;
		
		public:
			//Constructor
			Status();

			void poll(uint32_t time);

			void Status::batteryLow();
			void Status::batteryOK();
			void Status::armed();
			void Status::unarmed();
			void Status::commInterrupt();
			void Status::commOK();
	};
}
#endif
