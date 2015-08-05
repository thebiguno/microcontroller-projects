#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "menu/MainMenu.h"
#include "menu/LoadSamples.h"
#include "menu/MainVolume.h"

namespace digitalcave {

	class MenuState {
		public:
			static MainMenu mainMenu;
			static MainVolume mainVolume;
			static LoadSamples loadSamples;
	};
}

#endif