#include <stdio.h>
#include "main.h"

int main (void){
	timer_init(); 
	comm_init();
	control_init();

	uint64_t millis = timer_millis();
	uint16_t t = 0;
	uint8_t armed = 0;
    
	protocol_send_diag("controller reset");
	
	PORTD &= ~_BV(PIND5);		// off
	DDRD |= _BV(PIND5);			// set armed pin to output mode

	while (1){
		uint64_t curr_millis = timer_millis();
		dt = curr_millis - millis; // this is how long the last loop took
		millis = curr_millis;
		t += dt;

		protocol_poll();

		control_update();
		control_t control = control_read_analog();
		uint16_t button_state = control_button_state();
		uint16_t button_changed = control_button_state_changed();
		
		if ((button_state & POWER) && (button_changed & POWER)) { // rising edge, 0->1
			armed ^= 0x01;
			PORTD ^= _BV(PIND5); // toggle
			
			if (armed) {
				protocol_send_diag("armed");
				t = 0;
			} else {
				protocol_send_diag("disarmed");
				protocol_send_kill();
				control_reset_throttle();
			}
		}
		
		if (armed) {
			//Send control data
			if (t > 50) {
				char buf[50];
				snprintf(buf, 50, "%d %d %d %d", (int) (control.throttle * 100), (int) (control.pitch * 57.2957795), (int) (control.roll * 57.2957795), (int) (control.yaw * 57.2957795));
				protocol_send_diag(buf);
				protocol_send_control(control);
				t = 0;
			}
		} else {
			if (button_changed & RESET_ATTITUDE && button_state & RESET_ATTITUDE) { // rising edge, 0->1
				protocol_send_diag("reset attitude");
				protocol_send_reset_attitude();
			}
			if (button_changed & CALIBRATE && button_state & CALIBRATE) { // rising edge, 0->1
				protocol_send_diag("calibrate");
				protocol_send_calibrate();
			}
		}
    }
}

