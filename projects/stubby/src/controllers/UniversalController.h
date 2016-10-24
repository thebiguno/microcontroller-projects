#ifndef CONTROLLERS_UNIVERSAL_CONTROLLER_H
#define CONTROLLERS_UNIVERSAL_CONTROLLER_H

#include <stdint.h>
#include <FramedSerialProtocol.h>

namespace digitalcave {
	class Stubby; // forward declaration

	class UniversalController {
		public:
			UniversalController(Stubby* stubby);

			void dispatch(Stream* serial, FramedSerialMessage* message);

		private:
			Stubby* stubby;

	};
}

#endif
