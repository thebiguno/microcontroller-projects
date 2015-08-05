#ifndef MENU_H
#define MENU_H

#include <stdlib.h>

#include <Bounce.h>
#include <CharDisplay.h>
#include <Encoder.h>
#include <Hd44780_Teensy.h>

#include <vector>

#define PIN_RS							16
#define PIN_E							17
#define PIN_D4							0
#define PIN_D5							1
#define PIN_D6							2
#define PIN_D7							3

#define DISPLAY_ROWS					4
#define DISPLAY_COLS					20

#define ENC_PUSH						15
#define ENC_A							21
#define ENC_B							20

namespace digitalcave {

	class Menu {
	
		protected:
			static std::vector<Menu> menuStack;
			static char buf[21];	//Temp space for string operations

			static Hd44780_Teensy hd44780;
			static CharDisplay display;
			static Encoder encoder;
			static Bounce button;
			
			Menu();		//Prevent public instantiation
			
			int16_t encoderState;

		public:
			//Calls the handleAction() method for the current top on the stack
			static void poll();

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