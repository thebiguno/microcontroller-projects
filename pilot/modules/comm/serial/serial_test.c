/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../communication.h"

int main (void){
    comm_init();

    uint8_t new = 0;
    double command_values[4];
    uint8_t command_flags;
    double tuning_values[9];
    uint8_t tuning_id;

    //Main program loop
    while (1){
        _delay_ms(1000);

        // read and echo command
        new = comm_rx_command(command_values, &command_flags);
        if (new) {
            if (command_flags & _BV(0)) {
                vector_t vector = {command_values[1], command_values[2], command_values[3]};
                double motor[4] = {0,0,0,0};
                comm_tx_telemetry(vector, motor, command_flags);
            } else if (command_flags & _BV(1)) {
                vector_t vector = {0,0,0};
                comm_tx_telemetry(vector, command_values, command_flags);
            }
        }

        // read and echo tuning
        new = comm_rx_tuning(&tuning_id, tuning_values);
        if (new) {
            comm_tx_tuning(tuning_id, tuning_values);
        }
    }
}
