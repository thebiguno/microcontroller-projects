#include "main.h"

int main (void){
	uint8_t mode = MODE_STABLE;
	uint8_t armed = 0;
    
	DDRD |= _BV(PIND5);			// set armed pin to output mode
	PORTD |= _BV(PIND5);		// off

	comm_init();

	control_init();

	protocol_send_diag("controller reset");
	
	while (1){
		// PORTC ^= _BV(PINC0);		// turn led on

		protocol_poll();

		control_t control = control_read_analog();
		uint16_t buttons = control_read_buttons();
		
		if (buttons & POWER) {
			armed ^= 0x01;
			PORTD ^= _BV(PIND5);
			
			if (!armed) {
				// send a kill command
				control.throttle = 0;
				control.pitch = 0;
				control.roll = 0;
				control.yaw = 0;
				protocol_send_control(control);
			}
		}
		
		if (buttons & MODE_AEROBATIC) {
			mode = MODE_AEROBATIC;
		} else if (buttons & MODE_STABLE) {
			mode = MODE_STABLE;
		} else if (buttons & MODE_HOVER) {
			mode = MODE_HOVER;
		} else if (buttons & MODE_STABLE) {
			mode = MODE_STABLE;
		}
		
		if (!armed) {
			if (buttons & RESET_ATTITUDE) {
				protocol_send_reset_attitude();
			}
			if (buttons & CALIBRATE) {
				protocol_send_calibrate();
			}
		} else {
			if ((mode & MODE_AEROBATIC) == MODE_AEROBATIC) {
				// aerobatic / 3d mode (no limits on pitch and roll)
			} else if ((mode & MODE_STABLE) == MODE_SPORT) {
				// sport mode (roll limited to 30 deg, pitch limited to 45 deg)
				control.roll *= 0.3333;
				control.roll *= 0.5;
			} else if ((mode & MODE_HOVER) == MODE_HOVER) {
				// TODO this mode is pretty useless
				// hover mode (roll, pitch, yaw = 0)
				control.pitch = 0;
				control.roll = 0;
			} else {
				// stable mode (pitch and roll limited to 5 deg -- 0.0872664626 radians)
				control.pitch *= 0.0555;
				control.roll *= 0.0555;
			}
		
			//Send control data
			protocol_send_control(control);
		}
    }
}

