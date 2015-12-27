#ifndef MENU_H
#define MENU_H

#include <stdlib.h>

#include <ButtonTeensy.h>
#include <CharDisplay.h>
#include <Encoder.h>
#include <Hd44780_Teensy.h>

#include <vector>

#include "../hardware.h"

namespace digitalcave {

	class Menu {
		private:
			static Menu* current;
			static Encoder encoder;
			
			//The number of menu items for this menu.
			uint16_t menuCount;
	
		protected:
			static char buf[21];	//Temp space for string operations
			
			int16_t encoderState;
			
			//Constructor
			Menu(uint16_t menuCount);
			
			//Draw the arrow depending on the position offset
			void writeSelection(int8_t positionOffset);
			
		public:
			static Hd44780_Teensy* hd44780;
			static CharDisplay* display;
			static ButtonTeensy button;
			
			//All available menus
			static Menu* calibrateChannel;
			static Menu* calibrateChannelSelect;
			static Menu* kitSelect;
			static Menu* loadMappingsFromSD;
			static Menu* loadSamplesFromSD;
			static Menu* loadSamplesFromSerial;
			static Menu* mainMenu;
			static Menu* settings;
			static Menu* volumeHeadphones;
			static Menu* volumeLineIn;
			static Menu* volumePad;
			static Menu* volumePadSelect;
			static Menu* stats;
			
			//Calls the handleAction() method for the current menu
			static void poll();

			//Change state to the specified menu
			static void change(Menu* newMenu);
			
			//Change the menu count after initialization
			void setMenuCount(uint16_t menuCount);
			
			//Returns the currently selected menu item (or an offset from it), based on encoder state
			int16_t getMenuPosition(int8_t offset);
			//Sets the currently selected menu item, based on encoder state
			void setMenuPosition(int16_t position);
			//Returns 1, -1, or 0 depending on whether we are at the beginning, end, or middle of a menu
			int8_t getPositionOffset();

			//Check the encoder and button state to see if we need to do anything.
			// Update the display as needed.  Return NULL to stay in the same 
			// menu, or a Menu pointer to change to that menu item.
			virtual Menu* handleAction() = 0;
	};
}

#endif