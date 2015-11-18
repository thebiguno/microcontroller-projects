#ifndef MAINVOLUME_H
#define MAINVOLUME_H

#include <EEPROM/EEPROM.h>

#include "Menu.h"
#include "../hardware.h"

namespace digitalcave {

	class MainVolume : public Menu {
	
		private:
			uint8_t volume = 180;		//TODO save last volume to EEPROM
			
		public:
			MainVolume();
			Menu* handleAction();
	};
}

#endif