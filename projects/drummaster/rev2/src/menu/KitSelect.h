#ifndef KITSELECT_H
#define KITSELECT_H

#include <EEPROM/EEPROM.h>

#include "Menu.h"
#include "VolumePad.h"
#include "../pad/Pad.h"
#include "../util/Mapping.h"
#include "../hardware.h"

namespace digitalcave {

	class KitSelect : public Menu {
	
		private:
			int8_t lastKitIndex;
			Mapping mapping;
			
		public:
			static void loadKitIndexFromEeprom();
			static void saveKitIndexToEeprom();

			KitSelect();
			Menu* handleAction();
			Mapping* getMapping();
	};
}

#endif