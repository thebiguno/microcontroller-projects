#ifndef STATE_H
#define STATE_H

#include "Channel.h"
#include "Encoders.h"

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.

//Locked mode (default state):  Shows actual voltage / current measurements.
// Use encoder1 to scroll up and down (to see more channels than you have rows on display).
// Hold encoder1 to enter edit mode.
// Hold encoder2 to enter menu mode.
#define STATE_LOCKED					0

//Edit mode: pick a value to edit.
// Use encoder1 to scroll between values (2 values per channel: voltage / current)
// Press encoder1 to edit the selected item and move to Edit Item mode
// Hold encoder1 to return to lock Mode.
#define STATE_EDIT						1

//Edit Item mode: edit a previously selected value.
// Use encoder1 to increment / decrement the value by 1.
// Use encoder2 to increment / decrement the value by 0.01.
// Press encoder1 to return to Edit Mode.
#define STATE_EDIT_ITEM					2

//Menu mode: scroll through configuration menu items.
// Use encoder1 to scroll through menu items
// Hold encoder1 to edit the selected menu item
// Hold encoder2 to return to Normal mode
#define STATE_MENU						3

namespace digitalcave {

	class State {
		private:
			Encoders encoders;
			
			uint8_t state = 0;
			uint8_t scroll_channel = 0;	//From 0 to CHANNEL_COUNT - 1.  The currently selected channel.
			uint8_t scroll_value = 0;	//0 == Voltage or 1 == Current.  The currently selected value.  Used in conjunction with scroll_channel.
			uint8_t scroll_menu = 0;	//From 0 to MENU_COUNT - 1.  The currently selected menu item.
			
		public:
			State();
			
			void poll();
			
			uint8_t get_state();
			uint8_t get_scroll_channel();
			uint8_t get_scroll_value();
			uint8_t get_scroll_menu();
	};
}

#endif