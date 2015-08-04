#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "Menu.h"

namespace digitalcave {

	class MainMenu : public Menu {
	
		private:

		public:
			MainMenu(CharDisplay display, Encoder encoder, Bounce button);
			
			virtual void handleAction();
	};
}

#endif