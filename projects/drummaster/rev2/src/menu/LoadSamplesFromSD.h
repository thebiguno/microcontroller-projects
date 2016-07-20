#ifndef LOADSAMPLESFROMSD_H
#define LOADSAMPLESFROMSD_H

#include <EEPROM/EEPROM.h>
#include <SD.h>
#include <SerialFlash.h>

#include <vector>

#include "../hardware.h"
#include "../Pad.h"
#include "Menu.h"
#include "KitSelect.h"

namespace digitalcave {

	class LoadSamplesFromSD : public Menu {
	
		private:
			std::vector<String> folders;
			void ensureFoldersLoaded();
			
		public:
			LoadSamplesFromSD();
			Menu* handleAction();
	};
}

#endif