#include <util/delay.h>
#include "main.h"

#define WATCHDOG_ALERT	12

int main(){
	//********************
	// Module Init
	//********************

	status_init();
	comm_init();
	timer_init();
	gyro_init();
	esc_init();
	accel_init();  //We call accel_init after all other interrupt-driven modules, as it forces sei() (without which i2c will hang).
	pid_init();
	motor_init();
	attitude_init(gyro_get(), accel_get());
	
	if (esc_calibration_required()){
		esc_calibrate();
	}

	//Flash the startup status for 500ms (in direct status implementation, all 5 LEDs are on).
	status_error(STATUS_ERR_RESET);
	_delay_ms(500);
	//Statup status is cleared after all init is completed

	//********************
	// Variable Declarations
	//********************

	//Time variables	
	double seconds = 0;				//Last measured seconds (with close-to-microsecond resolution)
	double curr_seconds;			//Current seconds
	double dt;						//Time since last loop execution (how long the loop took; used for time sensitive calculations like Kalman)
	uint8_t t = 0;					//Time since last signal was received

	//State variables
	uint8_t cmd_type;				//Can be any of the flight command types (a superset of armed_type)
	uint8_t armed_type = 0x00;		//Will be either 'A' or 'M'; initialized to 0x00 until the first command is received.
	double flight_command_data[4];	//Data packet received with a flight command
	double throttle = 0.0;			//Throttle as sent from controller
	uint8_t throttle_back;			//Used for lost signal throttle decrease
	vector_t sp = { 0,0,0 };		// Attitude set point
	double motor[4];				// Motor set point
	
	uint8_t heartbeat_overflow = 0;	//When this overflows we do heartbeat and telemetry
		
	//Variables used in calculations
	vector_t g;						//Gyro readings
	vector_t a;						//Accel readings
	vector_t pv;					//Process Variable
	vector_t mv;					//Manipulated Variable (output of PID)
	
	
	//Clear startup status
	status_error(0x00);	

	//Main program loop
	while (1) {
		curr_seconds = timer_micros() / (double) 1000000;
		dt = curr_seconds - seconds;
		seconds = curr_seconds;
		
		protocol_poll();
		
		cmd_type = protocol_receive_flight_command(flight_command_data);
		if (cmd_type == 'A' || cmd_type == 'M') {
			armed_type = cmd_type;
			//Reset watchdog
			t = 0;
		}

		g = gyro_get();
		a = accel_get();
		pv = attitude(g, a, dt);					// compute PID process variable for x and y using Kalman

		if (armed_type == 'A') {						// attitude command
			status_set(STATUS_ARMED);
			
			throttle = flight_command_data[0];
			sp.x = flight_command_data[1];
			sp.y = flight_command_data[2];
			sp.z = flight_command_data[3];
			
			//Watchdog: check for communication timeouts
			if (t > WATCHDOG_ALERT) {
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
			
			//Reset PID if throttle is 0.
			if (throttle < 0.01) {
				pid_reset();
			}
			
			mv = pid_mv(sp, pv, dt);					// PID manipulated variable
			
			motor_percent(throttle, mv, motor);
			esc_set(motor);
		} else if (armed_type == 'M') {				// motor command
			for (uint8_t i = 0; i < 4; i++) {
				motor[i] = flight_command_data[i];
			}
			
			//Clear armed if all motor values are 0 (this is sent from control module when disarmed)
			if (motor[0] == 0 && motor[1] == 0 && motor[2] == 0 && motor[3] == 0) {
				status_clear(STATUS_ARMED);
			}
			else {
				status_set(STATUS_ARMED);
			}
			
			//Watchdog: check for communication timeouts
			if (t > WATCHDOG_ALERT) {
				status_clear(STATUS_ARMED);

				// kill the motors completely
				for (uint8_t i = 0; i < 4; i++) {
					motor[i] = 0;
				}
			}
			
			pid_reset();		// since pid isn't used for motor commands, take this opportunity to clear any accumulated error
			esc_set(motor);
			throttle = 0;
		}

		//********************
		// Intermittent I/O
		//********************

		heartbeat_overflow++;
		if (!heartbeat_overflow){
			//Watchdog timer; we assume this is about 250ms, so three seconds overflow 
			//will be t = WATCHDOG_ALERT (12), which we check for in the comm timeout code.
			t++;	
			
			status_toggle(STATUS_HEARTBEAT);
			
			status_clear(STATUS_MESSAGE_RX);
			status_clear(STATUS_MESSAGE_TX);
			
			protocol_send_battery(battery_level());
			protocol_send_telemetry(pv, motor);
			protocol_send_raw(g, a);
		}
	}
}

