#include <stdio.h>
#include "main.h"

int main (void){
	timer_init(); 
	comm_init();
	control_init();
	status_init();
	battery_init();

	uint64_t millis = timer_millis();

	//Used to send telemetry
	uint64_t millis_last_telemetry = millis;
	//Used to re-check battery level on controller
	uint64_t millis_last_status_battery = millis;
	//Used to update status
	uint64_t millis_last_status = millis;
	
	uint8_t armed = 0;
	uint64_t armed_time = 0;	//Running total; accumulated after disarmed command.
	uint64_t last_armed_time_start = 0;
    
	PORTD &= ~_BV(PIND5);		// off
	DDRD |= _BV(PIND5);			// set armed pin to output mode

	while (1){
		millis = timer_millis();

		protocol_poll();

		control_update();
		control_t control = control_read_analog();
		uint16_t button_state = control_button_state();
		uint16_t button_changed = control_button_state_changed();
		
		if ((button_state & POWER) && (button_changed & POWER)) { // rising edge, 0->1
			armed ^= 0x01;
			PORTD ^= _BV(PIND5); // toggle
			
			if (armed) {
				//Start counting...
				last_armed_time_start = millis;
			}
			else {
				//Add this armed 'chunk' to the running total
				armed_time = armed_time + (millis - last_armed_time_start);
				protocol_send_kill();
				control_reset_throttle();
			}
		}
		
		if ((button_state & TELEMETRY) && (button_changed & TELEMETRY)) { // rising edge, 0->1
			protocol_send_toggle_telemetry();
		}
		
		if (armed) {
			//Update armed time
			
		
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
		
		//TODO Update every 10 seconds or something longer... this is 1 second for debugging
		if ((millis - millis_last_status_battery) > 1000){
			millis_last_status_battery = millis;
			
			status_set_control_battery_level(battery_level());
		}
		
		
		//Update the status every quarter second
		if ((millis - millis_last_status) > 250){
			millis_last_status = millis;
			
//			status_set_pilot_battery_level(battery_state / (double) 0xff);
//			status_set_control_battery_level(battery_state / (double) 0xff);
//			status_set_telemetry(pitch, roll);
			status_set_throttle(control.throttle, armed);
			if (armed){
				status_set_armed_time(armed_time + (millis - last_armed_time_start));
			}
			else {
				status_set_armed_time(armed_time);
			}
//			status_set_motors(rand() / (double) RAND_MAX, rand() / (double) RAND_MAX, rand() / (double) RAND_MAX, rand() / (double) RAND_MAX);			
		}
    }
}

