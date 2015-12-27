#ifndef SETTINGS_H
#define SETTINGS_H

#include "Menu.h"

namespace digitalcave {

	class Settings : public Menu {
	
		private:

		public:
			Settings();
			Menu* handleAction();
	};
}

#endif