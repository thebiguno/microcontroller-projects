#ifndef PADSVOLUME_H
#define PADSVOLUME_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "Menu.h"
#include "CalibratePad.h"

namespace digitalcave {

	class PadsVolume : public Menu {
	
		private:
			uint8_t selectedPad;
			uint8_t volume;
			
		public:
			PadsVolume();
			Menu* handleAction();
	};
}

#endif