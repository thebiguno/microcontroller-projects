#ifndef DISPLAY_H
#define DISPLAY_H

#include <CharDisplay.h>
#include <Hd44780_Teensy.h>

#include "State.h"

#define PIN_RS							16
#define PIN_E							17
#define PIN_D4							0
#define PIN_D5							1
#define PIN_D6							2
#define PIN_D7							3

#ifndef DISPLAY_ROWS
#define DISPLAY_ROWS					4
#endif

#ifndef DISPLAY_COLS
#define DISPLAY_COLS					20
#endif

namespace digitalcave {

	class Display {
		private:
			Hd44780_Teensy hd44780;
			CharDisplay char_display;
			
			
		public:
			Display();
			
			void update(State state);
			
			void force_reset();
			
			CharDisplay get_char_display();

	};
}

#endif