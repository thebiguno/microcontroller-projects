#ifndef LOADFROMSD_H
#define LOADFROMSD_H

#include <EEPROM/EEPROM.h>
#include <SD.h>
#include <SerialFlash.h>

#include <vector>

#include "../hardware.h"
#include "../pad/Pad.h"
#include "Menu.h"

namespace digitalcave {

	class LoadFromSD : public Menu {
	
		private:
			std::vector<String> folders;
			void ensureFoldersLoaded();
			
		public:
			LoadFromSD();
			Menu* handleAction();
	};
}

#endif