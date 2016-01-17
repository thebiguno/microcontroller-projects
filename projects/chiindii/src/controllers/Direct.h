#ifndef DIRECT_H
#define DIRECT_H

#include <stdint.h>
#include <FramedSerialProtocol.h>

//Universal Controller messages are in 0x2X space...
#define MESSAGE_THROTTLE 0x20
#define MESSAGE_RATE 0x21
#define MESSAGE_ANGLE 0x22
#define MESSAGE_ZERO 0x23

namespace digitalcave {
	class Direct {
		private:
			
		public:
			Direct();

			void dispatch(FramedSerialMessage* message);
	};
}
#endif
