#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include <Bounce.h>
#include <Encoder.h>

#define ENC_PUSH					15
#define ENC_A						21
#define ENC_B						20

#define MENU_COUNT					4
#define ENCODER_STATE_COUNT			(MENU_COUNT * 2)

//We use a FSM to move between different modes.  Each mode will have differences in display
// and user interface.  The modes are listed below, along with comments describing what is
// happening and how to move to different states.
//TODO

namespace digitalcave {

	class State {
		private:
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