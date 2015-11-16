#ifndef MAINVOLUME_H
#define MAINVOLUME_H

#include "Menu.h"
#include "../Sample.h"

namespace digitalcave {

	class MainVolume : public Menu {
	
		private:
			float volume = 0.5;		//TODO save last volume to EEPROM
			
		public:
			MainVolume();
			Menu* handleAction();
	};
}

#endif