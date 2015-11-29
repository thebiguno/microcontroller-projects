#ifndef PAD_VOLUME_H
#define PAD_VOLUME_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "../hardware.h"
#include "Menu.h"

namespace digitalcave {

	class PadVolume : public Menu {
	
		private:
			uint8_t selectedPad;
			uint8_t adjusting;	//zero for top level, one for adjusting pad
			uint8_t volume;
			
		public:
			PadVolume();
			Menu* handleAction();
	};
}

#endif