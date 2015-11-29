#ifndef CALIBRATE_CHANNEL_SELECT_H
#define CALIBRATE_CHANNEL_SELECT_H

#include <Wire.h>

#include "../DrumMaster.h"
#include "Menu.h"
#include "CalibrateChannel.h"

namespace digitalcave {

	class CalibrateChannelSelect : public Menu {
	
		private:
			
		public:
			CalibrateChannelSelect();
			Menu* handleAction();
	};
}

#endif