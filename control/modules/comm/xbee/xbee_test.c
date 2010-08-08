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

	control_t control;
	uint8_t flags;

	//Main program loop
	while (1){
		_delay_ms(1000);

		control.pitch = 0.12345678;
		control.roll = 1.987654321;
		control.yaw = 0.0;
		control.throttle = 0.50;

		flags = 0xFE;

		comm_tx_ctrl(control, flags);
	}
}
