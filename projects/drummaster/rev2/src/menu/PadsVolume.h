#ifndef PADSVOLUME_H
#define PADSVOLUME_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "../hardware.h"
#include "Menu.h"

namespace digitalcave {

	class PadsVolume : public Menu {
	
		private:
			uint8_t selectedPad;
			uint8_t adjusting;	//zero for top level, one for adjusting pad
			uint8_t volume;
			
		public:
			PadsVolume();
			Menu* handleAction();
	};
}

#endif