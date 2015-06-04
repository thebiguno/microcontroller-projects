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
#define STATE_LOCKED					0

//Edit Item mode: edit a channel's voltage / current setpoints.
// Use encoder1 to increment / decrement the voltage.
// Use encoder2 to increment / decrement the current.
// Hold encoder1 to return to Locked Mode.
// Press encoder2 to increment the channel which is being edited.
#define STATE_EDIT_ITEM					1

//Menu mode: scroll through configuration menu items.
// Use encoder1 to scroll through menu items
// Hold encoder1 to edit the selected menu item
// Hold encoder2 to return to Normal mode
#define STATE_MENU						2


namespace digitalcave {

	class State {
		private:
			Encoders encoders;
			int16_t calculate_delta(int8_t encoder_movement);
			
			uint8_t state = 0;
			uint8_t scroll_channel = 0;	//From 0 to CHANNEL_COUNT - 1.  The currently selected channel.
			uint8_t scroll_menu = 0;	//From 0 to MENU_COUNT - 1.  The currently selected menu item.
			
		public:
			State();
			
			void poll();
			
			uint8_t get_state();
			uint8_t get_scroll_channel();
			uint8_t get_scroll_menu();
	};
}

#endif