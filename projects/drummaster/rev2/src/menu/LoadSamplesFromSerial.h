#ifndef LOADSAMPLESFROMSERIAL_H
#define LOADSAMPLESSFROMSERIAL_H

#include <EEPROM/EEPROM.h>
#include <SerialFlash.h>

#include <vector>

#include "../hardware.h"
#include "../pad/Pad.h"
#include "Menu.h"
#include "KitSelect.h"

namespace digitalcave {

	class LoadSamplesFromSerial : public Menu {
	
		private:
			Menu* flushError();
			
		public:
			LoadSamplesFromSerial();
			Menu* handleAction();
	};
}

#endif