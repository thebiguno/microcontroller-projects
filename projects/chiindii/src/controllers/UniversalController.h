#ifndef UNIVERSAL_CONTROLLER_H
#define UNIVERSAL_CONTROLLER_H

#include <UniversalControllerClient.h>
#include <FramedSerialProtocol.h>

#define MODE_FLIGHT			0
#define MODE_TUNING			1

namespace digitalcave {
	class Chiindii; // forward declaration

	class UniversalController {
		private:
			uint8_t mode;
			
			Chiindii *chiindii;
			
		public:
			UniversalController(Chiindii *chiindii);

			void dispatch(FramedSerialMessage* message);
	};
}
#endif

