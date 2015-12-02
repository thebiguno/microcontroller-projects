#ifndef LOADSAMPLES_H
#define LOADSAMPLES_H

#include <EEPROM/EEPROM.h>
#include <SD.h>
#include <SerialFlash.h>

#include <vector>

#include "../hardware.h"
#include "../pad/Pad.h"
#include "Menu.h"

namespace digitalcave {

	class LoadSamples : public Menu {
	
		private:
			std::vector<String> folders;
			void ensureFoldersLoaded();
			
		public:
			LoadSamples();
			Menu* handleAction();
	};
}

#endif