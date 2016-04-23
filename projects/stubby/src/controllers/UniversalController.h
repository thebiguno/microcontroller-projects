#ifndef UNIVERSAL_CONTROLLER_CLIENT_STUBBY_H
#define UNIVERSAL_CONTROLLER_CLIENT_STUBBY_H

#include "../Stubby.h"

namespace digitalcave {
	class UniversalController {
		public:
			UniversalController(Stubby* stubby);
			
			/*
			 * Called from the Stubby protocol dispatch function
			 */
			void dispatch(Stream* serial, FramedSerialMessage* message);
			
		private:
			Stubby* stubby;
			
	};
}

#endif