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
		
		if ((button_state & POWER) && (button_changed & POWER)) { // rising edge, 0->1
			armed ^= 0x01;
			PORTD ^= _BV(PIND5); // toggle
			
			if (!armed) {
				protocol_send_diag("disarmed");
				// send a kill command
				control.throttle = 0;
				control.pitch = 0;
				control.roll = 0;
				control.yaw = 0;
				protocol_send_control(control);
			} else {
				protocol_send_diag("armed");
			}
		}
		
		if (button_state & MODE_SPORT) {
			protocol_send_diag("sport mode");
			mode = MODE_SPORT;
		} else if (button_state & MODE_STABLE) {
			protocol_send_diag("stable mode");
			mode = MODE_STABLE;
		}
		
		if (!armed) {
			if (button_changed & RESET_ATTITUDE && button_state & RESET_ATTITUDE) { // rising edge, 0->1
				protocol_send_diag("reset attitude");
				protocol_send_reset_attitude();
			}
			if (button_changed & CALIBRATE && button_state & CALIBRATE) { // rising edge, 0->1
				protocol_send_diag("calibrate");
				protocol_send_calibrate();
			}
		} else {
			if ((mode & MODE_SPORT) == MODE_SPORT) {
				// sport mode (roll and roll limited to 45 deg -- 0.785398163 radians)
				control.pitch *= 0.5;
				control.roll *= 0.5;
			} else {
				// stable mode (pitch and roll limited to 5 deg -- 0.0872664626 radians)
				control.throttle *= 0.5;
				control.pitch *= 0.0555;
				control.roll *= 0.0555;
			}
		
			//Send control data
			if (dt > 50) {
				protocol_send_diag("*");
				protocol_send_control(control);
			}
		}
    }
}

