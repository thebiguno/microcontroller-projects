#ifndef VOLUME_LINE_IN_H
#define VOLUME_LINE_IN_H

#include <EEPROM/EEPROM.h>

#include "Menu.h"
#include "../hardware.h"

namespace digitalcave {

	class VolumeLineIn : public Menu {
	
		private:
			uint8_t volume;
			
		public:
			static void loadVolumeFromEeprom();
			static void saveVolumeToEeprom();
			
			VolumeLineIn();
			Menu* handleAction();
	};
}

#endif