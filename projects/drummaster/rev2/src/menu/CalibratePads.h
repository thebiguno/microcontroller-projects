#ifndef CALIBRATEPADS_H
#define CALIBRATEPADS_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "Menu.h"
#include "CalibratePad.h"

namespace digitalcave {

	class CalibratePads : public Menu {
	
		private:
			
		public:
			CalibratePads();
			Menu* handleAction();
	};
}

#endif