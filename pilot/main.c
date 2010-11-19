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
    double throttle;
    vector_t sp;
    double motor[4];
    uint8_t attitude_flags = 0x00;
    uint16_t command_flags = 0x0000;

    //Main program loop
    while (1) {
        comm_rx_attitude(&throttle, &sp, &attitude_flags);
        // comm_rx_motor(motor, &command_flags);
        // uint8_t tune_pid = comm_rx_pid(&p, &i, &d);
        // if (tune_pid) {
        //     pid_tune(p, i, d);
        // }
        // uint8_t tune_att = comm_rx_att(&att, &att_flags);
        // if (tune_att) {
        //     
        // }
        
        if (attitude_flags & 0x04 || command_flags & 0x0004) {
            gyro_calibrate();
        }
        if (attitude_flags & 0x08 || command_flags & 0x0008) {
            accel_calibrate();
        }
        if (attitude_flags & 0x10 || command_flags & 0x0010) {
            esc_calibrate();
        }
        
        vector_t g = gyro_get();
        vector_t a = accel_get();

        if (attitude_flags & 0x02 || command_flags & 0x02) {
            attitude_reset(&g, &a);
        }

        vector_t pv = attitude(g, a, dt);
        vector_t mv = pid_mv(sp, pv);

        if (attitude_flags & 0x01) {
            // armed by controller
            motor_percent(throttle, mv, motor);
            esc_set(1, motor);
        } else if (command_flags & 0x0001) {
            // armed by configuration
            esc_set(1, motor);
        } else {
            esc_set(0, motor);
        }
        
        if (command_flags & 0x0100) {
            comm_tx_telemetry(pv, motor, 0x00); // TODO flags
        }
        if (command_flags & 0x0200) {
//            comm_tx_pid_tuning(pid);
        }
        if (command_flags & 0x0400) {
        	double params[] = { 0.0 };
        	attitude_get_params(params);
        	comm_tx_attitude_tuning(params, attitude_get_id());
        }
        if (command_flags & 0x0800) {
            //comm_tx_raw(&g, &a);
        }
	}
}

