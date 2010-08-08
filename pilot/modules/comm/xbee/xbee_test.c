/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../communication.h"

int main (void){
	//Do setup here
	comm_init();

    uint8_t fresh_ctrl
    double ctrl_throttle;
    vector_t ctrl_sp;
	uint8_t ctrl_flags;
    uint8_t fresh_motor;
    double motor[4];
    uint16_t motor_flags;

	//Main program loop
	while (1){
		_delay_ms(1000);

        fresh_ctrl = comm_rx_ctrl(&ctrl_throttle, &ctrl_sp, &ctrl_flags);
        fresh_motor = comm_rx_motor(&motor, &motor_flags);

        // TODO receive pid tuning
        // TODO receive filter tuning
        
        // TODO send conditional on flags
        comm_tx_tm(&ctrl_sp, &motor, &ctrl_flags);
        // TODO send pid
	}
}
