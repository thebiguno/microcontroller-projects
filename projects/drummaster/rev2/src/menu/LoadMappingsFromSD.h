#ifndef LOADMAPPINGSFROMSD_H
#define LOADMAPPINGSFROMSD_H

#include <EEPROM/EEPROM.h>
#include <SD.h>
#include <SerialFlash.h>

#include <vector>

#include "../hardware.h"
#include "../pad/Pad.h"
#include "Menu.h"
#include "KitSelect.h"

namespace digitalcave {

	class LoadMappingsFromSD : public Menu {
	
		private:
			std::vector<String> folders;
			void ensureFoldersLoaded();
			
		public:
			LoadMappingsFromSD();
			Menu* handleAction();
	};
}

#endif