#include <util/delay.h>
#include "main.h"

int main(){
	//Enable internal pullup on MISO, and set pin to input mode
	*(&PORT_ESC_CALIBRATE_JUMPER - 0x1) &= ~_BV(PIN_ESC_CALIBRATE_JUMPER); //input mode
	PORT_ESC_CALIBRATE_JUMPER |= PIN_ESC_CALIBRATE_JUMPER;	//pullup on
	
	//If MISO is low, enter ESC calibration mode
	if (! (*(&PORT_ESC_CALIBRATE_JUMPER - 0x2) & _BV(PIN_ESC_CALIBRATE_JUMPER)) ){
		//Set first 4 LEDs on
		status_set(0xF);

		//Init ESC module
		esc_init();
		sei();	//Normally this is called in accel_init(), but here we do it manually.
		
		double motor[4];
		
		//Turn throttle high
		motor[0] = 1.0;
		motor[1] = 1.0;
		motor[2] = 1.0;
		motor[3] = 1.0;
		esc_set(motor);
		
		//Wait for MOSI to be high again (i.e., the user pulls the jumper from MOSI to gnd)
		while (! (*(&PORT_ESC_CALIBRATE_JUMPER - 0x2) & _BV(PIN_ESC_CALIBRATE_JUMPER)) ){
			;
		}
		
		//Set status to only heartbeat
		status_set(STATUS_HEARTBEAT);
		
		//Set throttle low
		motor[0] = 0.0;
		motor[1] = 0.0;
		motor[2] = 0.0;
		motor[3] = 0.0;		
		esc_set(motor);
		
		//End program
		while (1){
			;
		}
	}

	status_init();
	status_error(STATUS_ERR_RESET);
	_delay_ms(500);
	// error is cleared after all init is done
	
	uint64_t millis = 0;
	uint8_t armed = 0x00;
	uint64_t last_telemetry = 0;
	uint64_t last_status_clear = 0;
	uint64_t last_battery = 0;
	vector_t sp = { 0,0,0 };		// ATTITUDE set point
	double motor[4];				// MOTOR set point
//	double heading;					// heading hold
	uint8_t throttle_back;
	
	//Init all modules.  We call accel_init last as it forces sei().
	comm_init();
	timer_init();
	gyro_init();
	esc_init();
	accel_init();  //sei() is called in accel_init(), as it is needed for i2c.
	
	pid_init();
	attitude_init(gyro_get(), accel_get());
	
	status_error(0x00);
	
	uint16_t t = 0;
	double throttle = 0.0;
	
	//Main program loop
	while (1) {
		uint64_t curr_millis = timer_millis();
		uint64_t dt = curr_millis - millis;
		millis = curr_millis;
		t += dt;
		
		protocol_poll();
		
		double flight_command[4];
		uint8_t cmd_type = protocol_receive_flight_command(flight_command);
		if (cmd_type == 'A' || cmd_type == 'M') {
			armed = cmd_type;
			t = 0;
		}

		vector_t g = gyro_get();
		vector_t a = accel_get();
		vector_t pv = attitude(g, a, dt);			// compute PID process variable for x and y using Kalman

		if (armed == 'A') {							// armed by attitude command
			status_set(STATUS_ARMED);
			
			throttle = flight_command[0];
			sp.x = flight_command[1];
			sp.y = flight_command[2];
			sp.z = flight_command[3];
			if (t > 3000) {
				status_clear(STATUS_ARMED);
				
				// level out 
				sp.x = 0;
				sp.y = 0;
				sp.z = 0;
				
				// NOTE: this will go from full throttle to off in about two minutes
				throttle_back += dt;
				if (throttle > 0 && throttle_back >= 500) {
					// scale back throttle
					throttle_back = 0;
					throttle--;
				}
			}
			
			vector_t mv = pid_mv(sp, pv);			// PID manipulated variable
			
			motor_percent(throttle, mv, motor);
			esc_set(motor);
		} else if (armed == 'M') {					// armed by motor command
			status_set(STATUS_ARMED);
			for (uint8_t i = 0; i < 4; i++) {
				motor[i] = flight_command[i];
			}
			
			if (motor[0] == 0 && motor[1] == 0 && motor[2] == 0 && motor[3] == 0) {
				status_clear(STATUS_ARMED);
			}
			
			if (t > 3000) {
				status_clear(STATUS_ARMED);

				// kill the motors completely
				for (uint8_t i = 0; i < 4; i++) {
					motor[i] = 0;
				}
			}
			pid_reset();							// since pid isn't used for motor commands, take this opportunity to clear any accumulated error
			esc_set(motor);
			throttle = 0;
		}

		if (curr_millis - last_telemetry > 100){
			status_toggle(STATUS_HEARTBEAT);

			protocol_send_telemetry(pv, motor);
			protocol_send_raw(g, a);
			last_telemetry = curr_millis;			
		}

		if (curr_millis - last_status_clear > 500){
			status_clear(STATUS_MESSAGE_RX);
			status_clear(STATUS_MESSAGE_TX);
			
			last_status_clear = curr_millis;
		}
		
		if (curr_millis - last_battery > 5000){
			protocol_send_battery(battery_level());

			last_battery = curr_millis;
		}
	}
}

