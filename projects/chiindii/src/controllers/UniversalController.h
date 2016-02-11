#ifndef UNIVERSAL_CONTROLLER_H
#define UNIVERSAL_CONTROLLER_H

#include <UniversalControllerClient.h>
#include <FramedSerialProtocol.h>

namespace digitalcave {
	class Chiindii; // forward declaration

	class UniversalController {
		private:
			Chiindii *chiindii;
			
		public:
			UniversalController(Chiindii *chiindii);

			void dispatch(FramedSerialMessage* message);
	};
}
#endif

