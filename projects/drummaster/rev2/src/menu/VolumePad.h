#ifndef VOLUME_PAD_H
#define VOLUME_PAD_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "../hardware.h"
#include "Menu.h"

namespace digitalcave {

	class VolumePad : public Menu {
	
		private:
			
		public:
			static void loadPadVolumesFromEeprom();
			static void savePadVolumesToEeprom();
			
			int16_t value = -1;
			uint8_t pad = 0;

			VolumePad();
			Menu* handleAction();
	};
}

#endif