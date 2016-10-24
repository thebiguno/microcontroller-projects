#ifndef GENERAL_H
#define GENERAL_H

#include <stdint.h>
#include <FramedSerialProtocol.h>

//Generic messages are in 0x0X space...
#define MESSAGE_BATTERY							0x01
#define MESSAGE_STATUS							0x02
#define MESSAGE_DEBUG							0x03

namespace digitalcave {
	class Stubby; // forward declaration

	class General {
		public:
			General(Stubby* stubby);

			void dispatch(FramedSerialMessage* message);

		private:
			Stubby *stubby;
	};
}
#endif
