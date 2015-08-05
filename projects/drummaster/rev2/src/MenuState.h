#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "menu/Menu.h"

namespace digitalcave {

	class MenuState {
		private:
			Menu *current;
	
		public:
			MenuState(Menu* menu);
		
			//Calls the handleAction() method for the current menu
			void poll();

			//Change state to the specified menu
			void change(Menu* newMenu);
	};
}

#endif