#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include <Bounce.h>
#include <CharDisplay.h>
#include <Encoder.h>
#include <Hd44780_Teensy.h>
#include <SD.h>
#include <SerialFlash.h>

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

#define CS_FLASH						6
#define CS_SD							10

#define MENU_COUNT						4
#define ENCODER_STATE_COUNT				(MENU_COUNT * 2)

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.
//TODO

namespace digitalcave {

	class State {
		private:
			Hd44780_Teensy hd44780;
			CharDisplay char_display;
			Encoder encoder;
			Bounce button;
			uint8_t state = 0;
			
		public:
			State();
			
			void poll();
			
			uint8_t get_state();
	};
}

#endif