#ifndef MENU_H
#define MENU_H

#include <stdlib.h>
#include <stack>

#include <Bounce.h>
#include <CharDisplay.h>
#include <Encoder.h>

namespace digitalcave {

	class Menu {
	
		protected:
			static std::stack<Menu> menuStack;
			static char buf[21];	//Temp space for string operations

			Menu();		//Prevent public instantiation
			
			CharDisplay display;
			Encoder encoder;
			Bounce button;
			
			int16_t encoderState;

		public:
			//Calls the handleAction() method for the current top on the stack
			static void handleCurrent();

			//Go up one menu level if possible, and restore the state of the new
			// menu level.
			void up();
			
			//Navigate down to the specified menu, saving state in the current
			// one and placing the new menu on the stack.
			void down(Menu childMenu);
			
			//Check the encoder and button state to see if we need to do anything.
			// Update the display as needed.  Can call up() to return to higher 
			// level menus.
			virtual void handleAction();
	};
}

#endif