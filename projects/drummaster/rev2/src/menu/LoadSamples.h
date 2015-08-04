#ifndef LOADSAMPLES_H
#define LOADSAMPLES_H

#include <SD.h>
#include <SerialFlash.h>

#include <vector>

#include "Menu.h"

namespace digitalcave {

	class LoadSamples : public Menu {
	
		private:
			std::vector<String> folders;
			
		public:
			LoadSamples(CharDisplay display, Encoder encoder, Bounce button);
			virtual void handleAction();
	};
}

#endif