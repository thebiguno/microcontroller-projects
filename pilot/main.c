#include "main.h"


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

    uint8_t armed = 0x00;
    uint16_t dt = 0; // TODO need to update dt with the loop time
    double command[4];
    uint8_t command_flags;
    double tuning[9];
    uint8_t tuning_type;

    vector_t sp = { 0,0,0 };      // PID set point
    double motor[4];
    
    //Main program loop
    while (1) {
        comm_rx_command(command, &command_flags);
        uint8_t tuning_ct = comm_rx_tuning(&tuning_type, tuning);
        if (tuning_ct == 0) {
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

        if (command_flags & 0x01) { // attitude setpoint
			sp.x = command[1];
			sp.y = command[2];
			sp.z = command[3];
		} else if (command_flags & 0x02) { // motor setpoint
            sp.x = 0;
            sp.y = 0;
            sp.z = 0;
	    }
		
        vector_t pv = attitude(g, a, dt);           // PID process variable

        if (command_flags & 0x01) { // attitude setpoint
        	double throttle = command[0];
        	if (throttle < 0.001) {
                armed = 0x00;
        		esc_set(0, motor); // kill
        	} else {
                vector_t mv = pid_mv(sp, pv);       // PID manipulated variable
        		motor_percent(throttle, mv, motor);
				esc_set(1, motor);
        		armed = 0x01;
        	}
        } else if (command_flags & 0x02) { // motor setpoint
            esc_set(1, motor);
    		armed = 0x01;
        }
        
        if (command_flags & 0x04) { // reset attitude
            attitude_reset();
        }
        
        if (command_flags & 0x08) { // calibrate
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

        if (command_flags & 0x20) { // RTS telemetry
            comm_tx_telemetry(pv, motor, armed);
        }

	}
}

