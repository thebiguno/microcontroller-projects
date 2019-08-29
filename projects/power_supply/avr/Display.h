#ifndef DISPLAY_H
#define DISPLAY_H

#include <HD44780/CharDisplay.h>
#include <HD44780/Hd44780_Direct.h>

#include "State.h"

#ifndef DISPLAY_ROWS
#define DISPLAY_ROWS					4
#endif

#ifndef DISPLAY_COLS
#define DISPLAY_COLS					20
#endif

namespace digitalcave {

	class Display {
		private:
			Hd44780_Direct hd44780;
			CharDisplay char_display;

		public:
			Display();

			void update(State state);

			void force_reset();

			CharDisplay get_char_display();
	};
}

#endif
