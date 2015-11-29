#ifndef VOLUME_PAD_SELECT_H
#define VOLUME_PAD_SELECT_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "../hardware.h"
#include "Menu.h"
#include "VolumePad.h"

namespace digitalcave {

	class VolumePadSelect : public Menu {
	
		private:
			
		public:
			VolumePadSelect();
			Menu* handleAction();
	};
}

#endif