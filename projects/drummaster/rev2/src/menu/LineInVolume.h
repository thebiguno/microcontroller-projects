#ifndef LINEINVOLUME_H
#define LINEINVOLUME_H

#include <EEPROM/EEPROM.h>

#include "Menu.h"
#include "../hardware.h"

namespace digitalcave {

	class LineInVolume : public Menu {
	
		private:
			uint8_t volume;
			
		public:
			static void loadVolumeFromEeprom();
			static void saveVolumeToEeprom();
			
			LineInVolume();
			Menu* handleAction();
	};
}

#endif