#include "main.h"

int main (void){
	uint64_t millis = 0;
	uint8_t mode = MODE_STABLE;
	uint8_t armed = 0;
    
	DDRD |= _BV(PIND5);			// set armed pin to output mode
	PORTD |= _BV(PIND5);		// off

	comm_init();

	control_init();

	protocol_send_diag("controller reset");
	
	while (1){
		uint64_t curr_millis = timer_millis();
		uint64_t dt =+ (curr_millis - millis);
		millis = curr_millis;

		protocol_poll();

		control_update();
		control_t control = control_read_analog();
		uint16_t button_state = control_button_state();
		uint16_t button_changed = control_button_state_changed();
		
		if (button_changed & POWER && button_state & POWER) { // rising edge, 0->1
			armed ^= 0x01;
			PORTD ^= _BV(PIND5); // toggle
			
			if (!armed) {
				// send a kill command
				control.throttle = 0;
				control.pitch = 0;
				control.roll = 0;
				control.yaw = 0;
				protocol_send_control(control);
			}
		}
		
		if (button_state & MODE_AEROBATIC) {
			mode = MODE_AEROBATIC;
		} else if (button_state & MODE_STABLE) {
			mode = MODE_STABLE;
		} else if (button_state & MODE_HOVER) {
			mode = MODE_HOVER;
		} else if (button_state & MODE_STABLE) {
			mode = MODE_STABLE;
		}
		
		if (!armed) {
			if (button_changed & RESET_ATTITUDE && button_state & RESET_ATTITUDE) { // rising edge, 0->1
				protocol_send_reset_attitude();
			}
			if (button_changed & CALIBRATE && button_state & CALIBRATE) { // rising edge, 0->1
				protocol_send_calibrate();
			}
		} else {
			// TODO it would be best to simplify this to two modes
			// 		5 degree mode and 45 degree mode or something like that
			
			if ((mode & MODE_AEROBATIC) == MODE_AEROBATIC) {
				// aerobatic / 3d mode (no limits on pitch and roll)
			} else if ((mode & MODE_SPORT) == MODE_SPORT) {
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
			if (dt > 50) {
				protocol_send_control(control);
			}
		}
    }
}

