#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "Menu.h"
#include "../MenuState.h"

namespace digitalcave {

	class MainMenu : public Menu {
	
		private:

		public:
			MainMenu();
			void handleAction();
	};
}

#endif