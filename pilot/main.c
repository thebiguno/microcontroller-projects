#include "main.h"

int main(){

	//Heartbeat init
	*(&PORT_HEARTBEAT - 0x1) |= _BV(PIN_HEARTBEAT);
	PORT_HEARTBEAT |= _BV(PIN_HEARTBEAT);

	uint64_t millis = 0;
	uint8_t armed = 0x00;
	uint8_t rts_telemetry = 0x01;
	double command[4];
	uint8_t command_flags;
	double tuning[9];
	uint8_t tuning_type;
	
	uint64_t last_telemetry = 0;

	vector_t sp = { 0,0,0 };	  // PID set point
	double motor[4];

	//Init all modules.  We call accel_init last as it forces sei().
	comm_init();
	timer_init();
	gyro_init();
	accel_init();  //sei() is called in accel_init(), as it is needed for i2c.
	
	//TODO Remove later...
	gyro_calibrate();
	accel_calibrate();
	
	attitude_init(gyro_get(), accel_get());	   
	
	//Main program loop
	while (1) {
		//Heartbeat
		PORT_HEARTBEAT = PORT_HEARTBEAT ^ _BV(PIN_HEARTBEAT);
	
		uint64_t curr_millis = timer_millis();
		uint64_t dt =+ (curr_millis - millis);
		millis = curr_millis;
		uint8_t command_new_msg = comm_rx_command(command, &command_flags);
		uint8_t tuning_new_msg = comm_rx_tuning(&tuning_type, tuning);
		
		if (tuning_new_msg) {
			if (tuning_type == 'P') {
				vector_t kp = {tuning[0], tuning[3], tuning[6] };
				vector_t ki = {tuning[1], tuning[4], tuning[7] };
				vector_t kd = {tuning[2], tuning[5], tuning[8] };
				pid_set_params(&kp, &ki, &kd);
			} else if (tuning_type == attitude_get_id()) {
				attitude_set_params(tuning);
			}
		}
		if (command_new_msg) {
			if (command_flags & 0x01) {				// attitude setpoint command
				if (command[0] < 0.01) {
					armed = 0x00;
				} else {
					armed = 0x01;
				}
				sp.x = command[1];
				sp.y = command[2];
				sp.z = command[3];
			} else if (command_flags & 0x02) {		// motor setpoint command
				armed = 0x02;
				sp.x = 0;
				sp.y = 0;
				sp.z = 0;
			} else if (command_flags & 0x04) {		// reset attitude
				attitude_reset();
			} else if (command_flags & 0x08) {		// calibrate
				accel_calibrate();
				gyro_calibrate();
			} else if (command_flags & 0x10) {		// RTS tuning
				vector_t kp;
				vector_t ki;
				vector_t kd;
				pid_get_params(&kp, &ki, &kd);
				double pid[9] = { kp.x, ki.x, kd.x, kp.y, ki.y, kd.y, kp.z, ki.z, kd.z };
				comm_tx_tuning('P', pid);

				double params[9] = { 0.0 };
				attitude_get_params(params);
				comm_tx_tuning(attitude_get_id(), params);
			} else if (command_flags & 0x20) {		// RTS telemetry
			    // TODO with this logic, once telemetry is requested, it can't be turned off
				rts_telemetry = 0x01;
			} else if (command_flags & 0x80) {		// write EEPROM
				
			}			
			
			dt = 0;
		}

		vector_t g = gyro_get();
		vector_t a = accel_get();
		vector_t pv = attitude(g, a);				// PID process variable

		if (armed & 0x01) {							// armed by attitude command
			double throttle = command[0];
			if (dt > 3000) {
				// TODO level out and scale back throttle
			}
			vector_t mv = pid_mv(sp, pv);			// PID manipulated variable
			motor_percent(throttle, mv, motor);
			esc_set(motor);
		} else if (command_flags & 0x02) {			// armed by motor command
			if (dt > 3000) {
				// kill the motors completely
				for (uint8_t i = 0; i < 4; i++) {
					motor[i] = 0;
				}
			}
			esc_set(motor);
		}

		if (rts_telemetry) {						// RTS telemetry
			if (curr_millis - last_telemetry > 100){
				comm_tx_telemetry(pv, motor, armed);
				last_telemetry = curr_millis;
			}
		}
	}
}

