#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "Menu.h"

namespace digitalcave {

	class MainMenu : public Menu {
	
		private:

		public:
			MainMenu();
			Menu* handleAction();
	};
}

#endif