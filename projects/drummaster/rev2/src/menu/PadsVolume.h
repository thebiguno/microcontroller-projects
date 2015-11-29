#ifndef PADS_VOLUME_H
#define PADS_VOLUME_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "../hardware.h"
#include "Menu.h"
#include "PadVolume.h"

namespace digitalcave {

	class PadsVolume : public Menu {
	
		private:
			
		public:
			PadsVolume();
			Menu* handleAction();
	};
}

#endif