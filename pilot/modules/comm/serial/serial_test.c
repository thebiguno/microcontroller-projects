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

    uint8_t new_command
    double[4] command_values;
    uint8_t command_flags;

	//Main program loop
	while (1){
		_delay_ms(1000);

        new_command = comm_rx_command(&command_values, &command_flags);

        // TODO receive pid tuning
        // TODO receive filter tuning
        
        // TODO send conditional on flags
//        comm_tx_tm(&ctrl_sp, &motor, &ctrl_flags);
        // TODO send pid
	}
}
