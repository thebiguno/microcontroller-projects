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

    uint16_t dt = 0; // TODO need to update dt with the loop time
    double command[4];
    uint8_t command_flags;
    double tuning[9];
    uint8_t tuning_type;

    vector_t sp;
    double motor[4];

    //Main program loop
    while (1) {
        comm_rx_command(command, &command_flags);
        comm_rx_tuning(&tuning_type, tuning);
        
        vector_t g = gyro_get();
        vector_t a = accel_get();

        if ((command_flags & 0x01) == 0x01) { // attitude setpoint
			sp.x = command[1];
			sp.y = command[2];
			sp.z = command[3];
		}
		
        vector_t pv = attitude(g, a, dt);
        vector_t mv = pid_mv(sp, pv);

        uint8_t armed = 0x00;

        if ((command_flags & 0x01) == 0x01) { // attitude setpoint
        	double throttle = command[0];
        	if (throttle < 0.001) {
        		esc_set(0, motor); // kill
        	} else {
        		motor_percent(throttle, mv, motor);
				esc_set(1, motor);
        		armed = 0x01;
        	}
        } else if ((command_flags & 0x02) == 0x02) { // motor setpoint
            esc_set(1, motor);
    		armed = 0x01;
        }
        
        if ((command_flags & 0x04) == 0x04) { // reset attitude
       		// TODO this doesn't seem right
            attitude_reset(&g, &a);
        }
        
        if ((command_flags & 0x08) == 0x08) { // calibrate
            accel_calibrate();
            esc_calibrate();
        }

        if ((command_flags & 0x10) == 0x10) { // RTS tuning
        	double pid[9] = { 0.0 };
//        	pid_get_params(pid);
        	comm_tx_tuning(0x00, pid);

        	uint8_t type = attitude_get_id();
        	double params[9] = { 0.0 };
//        	attitude_get_params(params);
        	comm_tx_tuning(type, params);
        }

        if ((command_flags & 0x20) == 0x20) { // RTS telemetry
            comm_tx_telemetry(pv, motor, armed);
        }

	}
}

