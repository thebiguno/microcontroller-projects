#include <stdio.h>
#include "main.h"

int main (void){
	timer_init(); 
	comm_init();
	control_init();
	status_init();
	battery_init();

	uint64_t millis = timer_millis();
	uint64_t last_millis = millis;

	//Used to send telemetry
	uint64_t millis_last_telemetry = millis;
	//Used to update status
	uint64_t millis_last_status = millis;
	
	uint8_t armed = 0;
	uint64_t armed_time = 0;
    
	PORTD &= ~_BV(PIND5);		// off
	DDRD |= _BV(PIND5);			// set armed pin to output mode

	while (1){
		//Prevent the main loop from running more than every ms.
		while (millis <= last_millis){
			millis = timer_millis();
		}
		
		dt = millis - last_millis; // this is how long the last loop took; must be at least 1.
		last_millis = millis;

		protocol_poll();

		control_update();
		control_t control;
		if (armed){
			control = control_read_analog();
		}
		else {
			control.pitch = 0;
			control.roll = 0;
			control.yaw = 0;
			control.throttle = 0;
		}
		uint16_t button_state = control_button_state();
		uint16_t button_changed = control_button_state_changed();
		
		if ((button_state & POWER) && (button_changed & POWER)) { // rising edge, 0->1
			armed ^= 0x01;
			PORTD ^= _BV(PIND5); // toggle
			
			control_reset_throttle();
			
			if (!armed) {
				protocol_send_kill();
			}
		}
		
		if ((button_state & TELEMETRY) && (button_changed & TELEMETRY)) { // rising edge, 0->1
			protocol_send_toggle_telemetry();
		}
		
		if (armed) {
			//Update armed time
			armed_time += dt;
		
			//Send control data
			if ((millis - millis_last_telemetry) > 50) {
				protocol_send_control(control);
				millis_last_telemetry = millis;
			}
		} else {
			if (button_changed & RESET_ATTITUDE && button_state & RESET_ATTITUDE) { // rising edge, 0->1
				protocol_send_reset_attitude();
			}
			if (button_changed & CALIBRATE && button_state & CALIBRATE) { // rising edge, 0->1
				protocol_send_calibrate();
			}
		}
		
		//Update the status display
		if ((millis - millis_last_status) > 100){
			millis_last_status = millis;
			
			status_set_control_battery_level(battery_level());
//			status_set_pilot_battery_level(battery_state / (double) 0xff);
//			status_set_telemetry(pitch, roll);
			status_set_throttle(control.throttle, armed);
			status_set_armed_time(armed_time);
//			status_set_motors(rand() / (double) RAND_MAX, rand() / (double) RAND_MAX, rand() / (double) RAND_MAX, rand() / (double) RAND_MAX);			

			status_set_comm_state(protocol_comm_state(PROTOCOL_COMM_TX_PILOT), 
					protocol_comm_state(PROTOCOL_COMM_RX_PILOT), 
					protocol_comm_state(PROTOCOL_COMM_TX_PC), 
					protocol_comm_state(PROTOCOL_COMM_RX_PC));
					
			protocol_clear_comm_state();
		}
    }
}

