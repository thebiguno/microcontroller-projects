#include <stdio.h>
#include "main.h"

#define MODE_FLIGHT		0
#define MODE_CALIBRATE	1
#define MODE_PID		2
#define MODE_MOTOR		3
#define MODE_KALMAN		4

	
//Controller mode state.  Modes are as follows:
// 0: Flight mode (armed / disarmed, shows throttle, telemetry, battery, etc)
// 1: Calibration mode (calibrate gyro + accel)
// 2: PID mode (read / write PID values)
// 3: Motor tuning mode (read / write motor tuning values)
// 4: Kalman mode (read / write Kalman tuning values)
//Modes are incremented / decremented using Circle / Square buttons respectively.
int8_t mode = MODE_FLIGHT;

//Variables used in non-flight-mode modes
int8_t tuning_row, tuning_col;				//Reset to 0 when mode is changed; this is used to determine which element is being changed
vector_t pid_p, pid_i, pid_d;				//Volatile PID values; sent with X, modified with up / down pad
double motors[4] = {1.0, 1.0, 1.0, 1.0};	//Volatile motor tuning values; sent with X, modified with up / down pad
vector_t kalman_qa, kalman_qg, kalman_ra;	//Volatile Kalman values; sent with X, modified with up / down pad


void init_display(){
	if (mode == MODE_FLIGHT){
		status_init_mode_flight();
	}
	else if (mode == MODE_CALIBRATE){
		status_init_mode_calibrate();
	}
	else if (mode == MODE_PID){
		status_init_mode_pid();	
	}
	else if (mode == MODE_MOTOR){
		status_init_mode_motor();	
	}
	else if (mode == MODE_KALMAN){
		status_init_mode_kalman();
	}
}

void update_display(){
	if (mode == MODE_CALIBRATE){
		//Nop
	}
	else if (mode == MODE_PID){
		status_set_pid_values(tuning_col, tuning_row, pid_p, pid_i, pid_d);
	}
	else if (mode == MODE_MOTOR){
		status_set_motor_values(tuning_col, motors);			
	}
	else if (mode == MODE_KALMAN){
		status_set_kalman_values(tuning_col, tuning_row, kalman_qa, kalman_qg, kalman_ra);			
	}
}

void adjust_pid(int8_t value){
	//Adjust the selected value by the step
	if (tuning_col == 0){			//P
		if (tuning_row == 0) pid_p.x += value * 0.01;
		else if (tuning_row == 1) pid_p.y += value * 0.01;
	}
	else if (tuning_col == 1){		//I
		if (tuning_row == 0) pid_i.x += value * 0.0001;
		else if (tuning_row == 1) pid_i.y += value * 0.0001;	
	}
	else if (tuning_col == 2){		//D
		if (tuning_row == 0) pid_d.x += value * 0.01;
		else if (tuning_row == 1) pid_d.y += value * 0.01;
	}
	
	//Sanity checks
	if (pid_p.x < 0.0) pid_p.x = 0.0;
	if (pid_i.x < 0.0) pid_i.x = 0.0;
	if (pid_d.x < 0.0) pid_d.x = 0.0;

	if (pid_p.y < 0.0) pid_p.y = 0.0;
	if (pid_i.y < 0.0) pid_i.y = 0.0;
	if (pid_d.y < 0.0) pid_d.y = 0.0;

	if (pid_p.x > 1.0) pid_p.x = 1.0;
	if (pid_i.x > 0.1) pid_i.x = 0.1;
	if (pid_d.x > 5.0) pid_d.x = 5.0;

	if (pid_p.y > 1.0) pid_p.y = 1.0;
	if (pid_i.y > 0.1) pid_i.y = 0.1;
	if (pid_d.y > 5.0) pid_d.y = 5.0;
}

void adjust_motor(int8_t value){
	//Adjust the selected value by the step
	motors[tuning_col] += value * 0.01;	//1% step
	
	//Sanity checks; valid values are between 70% and 130%
	if (motors[tuning_col] < 0.7) motors[tuning_col] = 0.7;
	if (motors[tuning_col] > 1.3) motors[tuning_col] = 1.3;
}

void adjust_kalman(int8_t value){
	//Adjust the selected value by the step
	if (tuning_col == 0){			//E(alpha^2)
		if (tuning_row == 0) kalman_qa.x += value * 0.0001;
		else if (tuning_row == 1) kalman_qa.y += value * 0.0001;
	}
	else if (tuning_col == 1){		//E(bias^2)
		if (tuning_row == 0) kalman_qg.x += value * 0.0001;
		else if (tuning_row == 1) kalman_qg.y += value * 0.0001;	
	}
	else if (tuning_col == 2){		//Sz
		if (tuning_row == 0) kalman_ra.x += value * 0.01;
		else if (tuning_row == 1) kalman_ra.y += value * 0.01;
	}
	
	//Sanity checks
	if (kalman_qa.x < 0.0) kalman_qa.x = 0.0;
	if (kalman_qg.x < 0.0) kalman_qg.x = 0.0;
	if (kalman_ra.x < 0.0) kalman_ra.x = 0.0;

	if (kalman_qa.y < 0.0) kalman_qa.y = 0.0;
	if (kalman_qg.y < 0.0) kalman_qg.y = 0.0;
	if (kalman_ra.y < 0.0) kalman_ra.y = 0.0;

	if (kalman_qa.x > 0.1) kalman_qa.x = 0.1;
	if (kalman_qg.x > 0.1) kalman_qg.x = 0.1;
	if (kalman_ra.x > 5.0) kalman_ra.x = 5.0;

	if (kalman_qa.y > 0.1) kalman_qa.y = 0.1;
	if (kalman_qg.y > 0.1) kalman_qg.y = 0.1;
	if (kalman_ra.y > 5.0) kalman_ra.y = 5.0;
}

void check_buttons(uint16_t button_state, uint16_t button_changed){
	//Change mode using next (circle) and prev (square) buttons
	if (((button_state & MODE_NEXT) && (button_changed & MODE_NEXT)) || ((button_state & MODE_PREV) && (button_changed & MODE_PREV))) { // rising edge, 0->1
		mode += ((button_state & MODE_NEXT) ? 1 : -1);
		if (mode > MODE_KALMAN) mode = MODE_FLIGHT;
		if (mode < MODE_FLIGHT) mode = MODE_KALMAN;
		
		tuning_row = 0;
		tuning_col = 0;
		
		init_display();
		update_display();
	}
	//Commit (X) button is pressed
	else if ((button_state & MODE_COMMIT) && (button_changed & MODE_COMMIT)) {
		if (mode == MODE_CALIBRATE){
			protocol_send_calibrate();
		}
		else if (mode == MODE_PID){
			protocol_send_pid_tuning(pid_p, pid_i, pid_d);
		}
		else if (mode == MODE_MOTOR){
			protocol_send_motor_tuning(motors);
		}
		else if (mode == MODE_KALMAN){
			protocol_send_kalman_tuning(kalman_qa, kalman_qg, kalman_ra);
		}
		
		status_persist_values(1);
		_delay_ms(500);
		status_persist_values(0);
		
		update_display();
	}
	//Request (Triangle) button is pressed
	else if ((button_state & MODE_RESET) && (button_changed & MODE_RESET)) {
		//Regardless of what mode we are in, ask for tuning... it can't hurt anything if you ask for it in flight mode, etc.
		protocol_request_tuning();
	}
	//Up / down
	else if (button_state & VALUE_UP || button_state & VALUE_DOWN) {
		if (mode == MODE_PID){
			adjust_pid((button_state & VALUE_UP) ? 1 : -1);			
		}
		else if (mode == MODE_MOTOR){
			adjust_motor((button_state & VALUE_UP) ? 1 : -1);
		}
		else if (mode == MODE_KALMAN){
			adjust_kalman((button_state & VALUE_UP) ? 1 : -1);			
		}
		update_display();
	}
	//Left / right
	else if (((button_state & VALUE_NEXT) && (button_changed & VALUE_NEXT)) || ((button_state & VALUE_PREV) && (button_changed & VALUE_PREV))) {
		tuning_col = tuning_col + ((button_state & VALUE_NEXT) ? 1 : -1);
		if (mode == MODE_PID){
			if (tuning_col < 0){
				tuning_row--;
				tuning_col = 2;
			}
			else if (tuning_col >= 3){
				tuning_row++;
				tuning_col = 0;
			}
			
			if (tuning_row < 0){
				tuning_row = 1;
			}
			else if (tuning_row >= 2){
				tuning_row = 0;
			}
		}
		else if (mode == MODE_MOTOR){
			if (tuning_col < 0){
				tuning_col = 3;
			}
			else if (tuning_col >= 4){
				tuning_col = 0;
			}
		}
		else if (mode == MODE_KALMAN){
			if (tuning_col < 0){
				tuning_row--;
				tuning_col = 2;
			}
			else if (tuning_col >= 3){
				tuning_row++;
				tuning_col = 0;
			}
			
			if (tuning_row < 0){
				tuning_row = 1;
			}
			else if (tuning_row >= 2){
				tuning_row = 0;
			}
		}
		update_display();
	}
}

int main (void){
	timer_init(); 
	comm_init();
	control_init();
	status_init();
	status_error_init();
	battery_init();
	sei();	//We have NO_INTERRUPT_ENABLE in the Makefile, so that we only call sei() once.

	uint64_t millis = timer_millis();
	uint64_t last_millis = millis;

	//Used to send control data (throttle / pitch / roll / yaw)
	uint64_t millis_last_control = millis;
	//Used to update status
	uint64_t millis_last_status = millis;
	//Used to update battery
	uint64_t millis_last_battery = millis;	
	//Used for updating telemetry 
	double buffer_array[] = {0,0,0,0};
	vector_t buffer_vector;
	
	uint8_t armed = 0;
	uint64_t armed_time = 0;
	    
    //Init the text / graphics used in flight mode
    status_init_mode_flight();
    
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
		
		//Check for tuning requests from the poll'd protocol
		protocol_get_pid_tuning(&pid_p, &pid_i, &pid_d);
		protocol_get_motor_tuning(motors);
		protocol_get_pid_tuning(&kalman_qa, &kalman_qg, &kalman_ra);

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
			mode = MODE_FLIGHT;	//If you arm, you cancel out of any pending changes and go to flight mode.
			init_display();
			
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
		
			//Send current control data every 50ms if armed.
			if ((millis - millis_last_control) > 50) {
				protocol_send_control(control);
				millis_last_control = millis;
			}
		} else {
			check_buttons(button_state, button_changed);
		
			//Send kill data every 200ms if not armed; this will prevent a missed 
			// message from preventing the copter from not disarming.
			if ((millis - millis_last_control) > 200) {
				protocol_send_kill();
				millis_last_control = millis;
			}
		}
		
		//Update the status display
		if ((millis - millis_last_status) > 200){
			millis_last_status = millis;

			if (mode == MODE_FLIGHT){			
				//Control Battery
				status_set_control_battery_level(battery_level());
				status_error_battery(battery_level() < 0.2);
	
				//Pitch / Roll
				buffer_vector = protocol_get_vector();
				status_set_telemetry(buffer_vector.x, buffer_vector.y);
	
				//Motors
				protocol_get_motors(buffer_array);
				status_set_motors(buffer_array[0], buffer_array[1], buffer_array[2], buffer_array[3]);
				
				//Throttle / Armed / Time
				status_set_throttle(control.throttle, armed);
				status_set_armed_time(armed_time);
	
				//Rx / Tx
				status_set_comm_state(protocol_comm_state(PROTOCOL_COMM_TX), 
						protocol_comm_state(PROTOCOL_COMM_RX));
				protocol_clear_comm_state();
			}
			else {
				//In case of incoming tuning requests
				update_display();
			}
		}
		
		if ((millis - millis_last_battery) > 2000){
			if (mode == MODE_FLIGHT){			
				millis_last_battery = millis;
				
				buffer_array[0] = protocol_get_battery();
				status_set_pilot_battery_level(buffer_array[0]);
				status_error_battery(buffer_array[0] < 0.2 && buffer_array[0] >= 0);
			}
		}
    }
}

