#ifndef AbstractPsx_h
#define AbstractPsx_h

#include <stdint.h>

//These are our button constants
#define PSB_SELECT      0x0001
#define PSB_L3          0x0002
#define PSB_R3          0x0004
#define PSB_START       0x0008
#define PSB_PAD_UP      0x0010
#define PSB_PAD_RIGHT   0x0020
#define PSB_PAD_DOWN    0x0040
#define PSB_PAD_LEFT    0x0080
#define PSB_L2          0x0100
#define PSB_R2          0x0200
#define PSB_L1          0x0400
#define PSB_R1          0x0800
#define PSB_TRIANGLE    0x1000
#define PSB_CIRCLE      0x2000
#define PSB_CROSS       0x4000
#define PSB_SQUARE      0x8000

//These are stick values
#define PSS_RX 5
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8


namespace digitalcave {
	class AbstractPsx {
	private:
		uint8_t data[21];

		uint16_t buttons_state;
		uint16_t buttons_changed;
		
		virtual void sendCommand(uint8_t send_data[], uint8_t size);
		
	public:
		/* Reads the gamepad.  Call this whenever you want updated state. */
		void poll();
		
		/* Gets a bitmask of all buttons that are pressed. */
		uint16_t buttons();
		/* Gets a bitmask of all buttons that have changed state since the last call to buttons. */
		uint16_t changed();
		/* Gets the state of one button. */
		uint8_t button(uint16_t button);
		/* Gets the value of a stick axis. */
		uint8_t stick(unsigned int stick);
	};
}

#endif
