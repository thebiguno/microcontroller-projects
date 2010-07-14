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
	init_communication();

	control_t control;
	uint8_t mode;

	//Main program loop
	while (1){

		_delay_ms(1000);

		control.pitch = 0.12345678;
		control.roll = 1.987654321;
		control.yaw = 0.0;
		control.throttle = 3.141592653589793;

		mode = 0xFE;
		
		communication_send(control, mode);
	}
}
