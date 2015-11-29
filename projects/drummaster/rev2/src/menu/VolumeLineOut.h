#ifndef VOLUME_LINE_OUT_H
#define VOLUME_LINE_OUT_H

#include <EEPROM/EEPROM.h>

#include "Menu.h"
#include "../hardware.h"

namespace digitalcave {

	class VolumeLineOut : public Menu {
	
		private:
			uint8_t volume;
			
		public:
			static void loadVolumeFromEeprom();
			static void saveVolumeToEeprom();
			
			VolumeLineOut();
			Menu* handleAction();
	};
}

#endif