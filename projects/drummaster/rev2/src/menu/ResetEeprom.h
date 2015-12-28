#ifndef RESET_EEPROM_H
#define RESET_EEPROM_H

#include <EEPROM/EEPROM.h>

#include "../hardware.h"
#include "../pad/Pad.h"
#include "Menu.h"
#include "KitSelect.h"

namespace digitalcave {

	class ResetEeprom : public Menu {
	
		private:
			
		public:
			ResetEeprom();
			Menu* handleAction();
	};
}

#endif