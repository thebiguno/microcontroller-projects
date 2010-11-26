#include "main.h"
#include "../lib/timer/timer.h"

void set_data(char *data){
/*
	accel_set_data(data);
	attitude_set_data(data);
	gyro_set_data(data);
	motor_set_data(data);
	pid_set_data();
*/
}

void send_debug_data(uint8_t *data){
    //TODO send 
}

int main(){
    //Do setup here

    uint64_t millis = 0;
    uint8_t armed = 0x00;
    uint8_t rts_telemetry = 0x00;
    double command[4];
    uint8_t command_flags;
    double tuning[9];
    uint8_t tuning_type;

    vector_t sp = { 0,0,0 };      // PID set point
    double motor[4];
    
    //Main program loop
    while (1) {
        uint64_t curr_millis = timer_millis();
        uint64_t dt =+ (curr_millis - millis);
        millis = curr_millis;
        uint8_t command_new_msg = comm_rx_command(command, &command_flags);
        uint8_t tuning_new_msg = comm_rx_tuning(&tuning_type, tuning);
        if (tuning_new_msg) {
            if (tuning_type == 0x00) {
                vector_t kp = {tuning[0], tuning[3], tuning[6] };
                vector_t ki = {tuning[1], tuning[4], tuning[7] };
                vector_t kd = {tuning[2], tuning[5], tuning[8] };
                pid_set_params(kp, ki, kd);
            } else {
                if (tuning_type == attitude_get_id()) {
                    attitude_set_params(tuning);
                }
            }
        }
        
        vector_t g = gyro_get();
        vector_t a = accel_get();

        if (command_new_msg && command_flags & 0x01) { // attitude setpoint command
            if (command[0] < 0.001) {
                armed = 0x00;
            } else {
                armed = 0x01;
            }
			sp.x = command[1];
			sp.y = command[2];
			sp.z = command[3];
			
			if (command_flags & 0x10) { // RTS tuning
                rts_telemetry = 0x01;
            } else {
                rts_telemetry = 0x00;
            }
            dt = 0;
		} else if (command_new_msg && command_flags & 0x02) { // motor setpoint command
            armed = 0x02;
            sp.x = 0;
            sp.y = 0;
            sp.z = 0;

            if (command_flags & 0x20) { // RTS telemetry
                rts_telemetry = 0x01;
            } else {
                rts_telemetry = 0x00;
            }
            dt = 0;
	    }
		
        vector_t pv = attitude(g, a);               // PID process variable

        if (armed & 0x01) {                         // armed by attitude command
        	double throttle = command[0];
        	if (dt > 3000) {
        	    // TODO level out and scale back throttle
        	}
            vector_t mv = pid_mv(sp, pv);           // PID manipulated variable
    		motor_percent(throttle, mv, motor);
			esc_set(motor);
        } else if (command_flags & 0x02) {          // armed by motor command
            if (dt > 3000) {
                // kill the motors completely
                motor[0] = 0;
                motor[1] = 0;
                motor[2] = 0;
                motor[3] = 0;
            }
            esc_set(motor);
        }
        
        if (command_new_msg && command_flags & 0x04) { // reset attitude
            attitude_reset();
        }
        
        if (command_new_msg && command_flags & 0x08) { // calibrate
            accel_calibrate();
            gyro_calibrate();
            esc_calibrate();
        }
        
        if (command_flags & 0x10) { // RTS tuning
            vector_t kp;
            vector_t ki;
            vector_t kd;
        	pid_get_params(kp, ki, kd);
        	double pid[9] = { kp.x, ki.x, kd.x, kp.y, ki.y, kd.y, kp.z, ki.z, kd.z };
        	comm_tx_tuning(0x00, pid);

        	uint8_t type = attitude_get_id();
        	double params[9] = { 0.0 };
        	attitude_get_params(params);
        	comm_tx_tuning(type, params);
        }
        
        if (command_new_msg && command_flags & 0x80) { // write EEPROM

        }            

        if (rts_telemetry) { // RTS telemetry
            comm_tx_telemetry(pv, motor, armed);
        }
	}
}

