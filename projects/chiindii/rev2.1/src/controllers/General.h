#ifndef GENERAL_H
#define GENERAL_H

#include <stdint.h>
#include <FramedSerialProtocol.h>

//Generic messages are in 0x0X space...
#define MESSAGE_BATTERY							0x01
#define MESSAGE_STATUS							0x02
#define MESSAGE_DEBUG							0x03

namespace digitalcave {
	class Chiindii; // forward declaration
	
	class General {
		private:
			Chiindii *chiindii;

		public:
			General(Chiindii *chiindii);

			void dispatch(FramedSerialMessage* message);
	};
}
#endif
