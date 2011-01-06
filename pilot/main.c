#include "main.h"

int main(){

	uint64_t millis = 0;
	uint8_t armed = 0x00;
	uint64_t last_telemetry = 0;
	vector_t sp = { 0,0,0 };		// ATTITUDE set point
	double motor[4];				// MOTOR set point
	
	//Init all modules.  We call accel_init last as it forces sei().
	status_init();
	comm_init();
	timer_init();
	gyro_init();
	accel_init();  //sei() is called in accel_init(), as it is needed for i2c.
	
	pid_init();
	attitude_init(gyro_get(), accel_get());	   
	
	//Main program loop
	while (1) {
		uint64_t curr_millis = timer_millis();
		uint64_t dt =+ (curr_millis - millis);
		millis = curr_millis;

		protocol_poll();
		
		double flight_command[4];
		uint8_t cmd_type = protocol_receive_flight_command(flight_command);
		if (cmd_type == 'A' || cmd_type == 'M') {
			armed = cmd_type;
			dt = 0;
		}

		vector_t g = gyro_get();
		vector_t a = accel_get();
		vector_t pv = attitude(g, a);				// PID process variable

		if (armed == 'A') {							// armed by attitude command
			status_set(STATUS_ARMED);
			
			double throttle = flight_command[0];
			sp.x = flight_command[1];
			sp.y = flight_command[2];
			sp.z = flight_command[3];
			if (dt > 3000) {
				status_clear(STATUS_ARMED);
				
				// TODO level out and scale back throttle
			}
			vector_t mv = pid_mv(sp, pv);			// PID manipulated variable
			motor_percent(throttle, mv, motor);
			esc_set(motor);
		} else if (armed == 'M') {					// armed by motor command
			status_set(STATUS_ARMED);
			
			if (dt > 3000) {
				status_clear(STATUS_ARMED);
				// kill the motors completely
				
				for (uint8_t i = 0; i < 4; i++) {
					flight_command[i] = 0;
				}
			}
			esc_set(flight_command);
		}

		if (curr_millis - last_telemetry > 250){
			status_toggle(STATUS_HEARTBEAT);

			protocol_send_telemetry(pv, motor);
			last_telemetry = curr_millis;
		}
	}
}

