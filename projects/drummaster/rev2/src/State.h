#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include <Bounce.h>
#include <CharDisplay.h>
#include <Encoder.h>
#include <Hd44780_Teensy.h>
#include <SD.h>
#include <SerialFlash.h>

#include "DrumMaster.h"
#include "util/FileUtil.h"

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

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.

//Default mode.  Switch between different menu options.
#define STATE_MENU						0

//Change master volume.
#define STATE_MAIN_VOLUME				1
//Change channel volume
#define STATE_CHANNEL_VOLUME			2

//Search folders mode.  Turning the encoder will scroll through all directories immediately below
// the root folder on the SD card.
#define STATE_SCAN_FOLDERS				3


#define MENU_MAIN_VOLUME				0
#define MENU_CHANNEL_VOLUME				1
#define MENU_LOAD_SAMPLES				2

#define MENU_COUNT						3

namespace digitalcave {

	class State {
		private:
			Hd44780_Teensy hd44780;
			CharDisplay display;
			Encoder encoder;
			Bounce button;
			uint8_t state = STATE_MENU;
			uint8_t menu = MENU_MAIN_VOLUME;
			float volume = 0.5;		//TODO save last volume to EEPROM
			
		public:
			State();
			
			void poll();
			
			uint8_t get_state();
			
			float get_volume();
	};
}

#endif