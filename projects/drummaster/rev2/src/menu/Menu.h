#ifndef MENU_H
#define MENU_H

#include <stdlib.h>

#include <Bounce.h>
#include <CharDisplay.h>
#include <Encoder.h>
#include <Hd44780_Teensy.h>

#include <vector>

#include "../hardware.h"

namespace digitalcave {

	class Menu {
		private:
			static Menu* current;
	
		protected:
			static char buf[21];	//Temp space for string operations
			
			//Constructor
			Menu();

		public:
			static Hd44780_Teensy* hd44780;
			static CharDisplay* display;
			static Encoder encoder;
			static Bounce button;
			
			//All available menus
			static Menu* mainMenu;
			static Menu* volumeLineIn;
			static Menu* volumeLineOut;
			static Menu* volumePad;
			static Menu* volumePadSelect;
			static Menu* loadSamples;
			static Menu* calibrateChannel;
			static Menu* calibrateChannelSelect;
			
			//Calls the handleAction() method for the current menu
			static void poll();

			//Change state to the specified menu
			static void change(Menu* newMenu);
			
			int16_t encoderState;

			//Check the encoder and button state to see if we need to do anything.
			// Update the display as needed.  Return NULL to stay in the same 
			// menu, or a Menu pointer to change to that menu item.
			virtual Menu* handleAction() = 0;
	};
}

#endif