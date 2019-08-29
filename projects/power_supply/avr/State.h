#ifndef STATE_H
#define STATE_H

#include "Channel.h"
#include "Encoders.h"

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.

//Locked mode (default state):  Shows actual voltage / current measurements.
// Use encoder1 to scroll up and down (to see more channels than you have rows on display).
// Hold encoder1 to enter edit item mode.
// Hold encoder2 to enter menu mode.
#define STATE_LOCKED					0x00

//Edit Item mode: edit a channel's voltage / current setpoints.
// Use encoder1 to increment / decrement the voltage.
// Use encoder2 to increment / decrement the current.
// Hold encoder1 to return to Locked Mode.
// Press encoder2 to increment the channel which is being edited.
#define STATE_EDIT_ITEM					0x01

//Menu mode: allow calibration, DFU upload, etc.
// Use encoder1 to select item, then press to select.
#define STATE_MENU						0x02

//Calibration modes: allow calibration of the currently selected channel.
#define STATE_CALIBRATE_VOLTAGE			0x03
#define STATE_CALIBRATE_CURRENT			0x04

//Allow user to define the startup set points
#define STATE_STARTUP_SETPOINT			0x05

#define STATE_FIRMWARE					0x06


//The menu indices for various functions
#define MENU_CALIBRATE_VOLTAGE			0x00
#define MENU_CALIBRATE_CURRENT			0x01
#define MENU_STARTUP_SETPOINT			0x02
#define MENU_FIRMWARE					0x03

#define MENU_SIZE						0x04

namespace digitalcave {

	class State {
		private:
			Encoders encoders;
			int16_t calculate_delta_a(int8_t encoder_movement);
			int16_t calculate_delta_v(int8_t encoder_movement);

			uint8_t state = 0;
			uint8_t scroll_channel = 0;	//From 0 to CHANNEL_COUNT - 1.  The currently selected channel.
			uint8_t scroll_menu = 0;	//From 0 to MENU_COUNT - 1.  The currently selected menu item.
			uint8_t calibrate_index = 0;	//From 0 to CALIBRATION_COUNT - 1.  The currently selected calibration index.

		public:
			State();

			void poll();

			uint8_t get_state();
			uint8_t get_scroll_channel();
			uint8_t get_scroll_menu();
			uint8_t get_calibrate_index();
	};
}

#endif
