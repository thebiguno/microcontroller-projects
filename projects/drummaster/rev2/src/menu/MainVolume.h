#ifndef MAINVOLUME_H
#define MAINVOLUME_H

#include <Audio.h>

#include "Menu.h"

namespace digitalcave {

	class MainVolume : public Menu {
	
		private:
			float volume = 0.5;		//TODO save last volume to EEPROM
			
		public:
			MainVolume(CharDisplay display, Encoder encoder, Bounce button);
			virtual void handleAction();
	};
}

#endif