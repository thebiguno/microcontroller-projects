#ifndef STATE_H
#define STATE_H

#include <bootloader/bootloader.h>
#include <Button/ButtonAVR.h>
#include <timer/timer.h>
#include <I2CAVR.h>
#include <DS3231.h>

#include "Light.h"
#include "Encoder.h"

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.

//Time mode (default state).  Shows the time.  Press the button to toggle light, turn encoder to
// adjust light brightness.  Hold button to enter menu.
#define STATE_TIME					0x00

//Menu mode: allow setting time, alarm, etc.
#define STATE_MENU						0x01

//The menu indices for various functions
#define MENU_SET_ALARM					0x00
#define MENU_SET_TIME					0x01

#define MENU_SIZE						0x02

namespace digitalcave {

	class State {
		private:
			I2CAVR i2c;
			DS3231 calendar;
			Encoder encoder;
			ButtonAVR button;

			uint8_t state = 0;
			int16_t light_brightness = 0;
			uint8_t menu_item = 0;	//From 0 to MENU_COUNT - 1.  The currently selected menu item.

		public:
			State();

			void poll();

			ds3231_time_t get_time();
			uint8_t get_state();
			uint8_t get_menu_item();
			int16_t get_light_brightness();
	};
}

#endif
