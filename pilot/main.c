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
    uint8_t cmd_flags = 0x00;
    uint16_t motor_flags = 0x0000;

    //Main program loop
    while (1) {
        comm_rx_ctrl(&throttle, &sp, &cmd_flags);
        // comm_rx_motor(motor, &motor_flags);
        // uint8_t tune_pid = comm_rx_pid(&p, &i, &d);
        // if (tune_pid) {
        //     pid_tune(p, i, d);
        // }
        // uint8_t tune_att = comm_rx_att(&att, &att_flags);
        // if (tune_att) {
        //     
        // }
        
        if (cmd_flags & 0x04 || motor_flags & 0x04) {
            gyro_calibrate();
        }
        if (cmd_flags & 0x08 || motor_flags & 0x08) {
            accel_calibrate();
        }
        if (cmd_flags & 0x10 || motor_flags & 0x10) {
            esc_calibrate();
        }
        
        vector_t g = gyro_get();
        vector_t a = accel_get();

        if (cmd_flags & 0x02 || motor_flags & 0x02) {
            attitude_reset(&g, &a);
        }

        vector_t pv = attitude(g, a, dt);
        vector_t mv = pid_mv(sp, pv);

        if (cmd_flags & 0x01) {
            // armed by controller
            motor_percent(throttle, mv, motor);
            esc_set(1, motor);
        } else if (motor_flags & 0x0001) {
            // armed by configuration
            esc_set(1, motor);
        } else {
            esc_set(0, motor);
        }
        
        if (cmd_flags & 0x0100) {
            // comm_tx_pid(pid); // TODO expose pid module values
        }
        if (cmd_flags & 0x0200) {
            comm_tx_tm(pv, motor, 0x00); // TODO flags
        }
        if (cmd_flags & 0x0300) {
            //comm_tx_raw(&g, &a);
        }
	}
}

