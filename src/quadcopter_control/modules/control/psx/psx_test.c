/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../control.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/serial/serial.h"

char temp[32];

control_t control, last_control;

int main (void){
	//Do setup here

	serial_init(9600, 8, 0, 1);

	init_control_hardware();

	//Main program loop
	while (1){

		_delay_ms(10);

		
		control = get_control();

		if (control.pitch != last_control.pitch 
				|| control.roll != last_control.roll 
				|| control.yaw != last_control.yaw 
				|| control.throttle != last_control.throttle){
			last_control = control;

			sprintf(temp, "%g", control.pitch);
			serial_write_s(temp);
			serial_write_s(", ");
			sprintf(temp, "%g", control.roll);
			serial_write_s(temp);
			serial_write_s(", ");
			sprintf(temp, "%g", control.yaw);
			serial_write_s(temp);
			serial_write_s(", ");
			sprintf(temp, "%g", control.throttle);
			serial_write_s(temp);
			serial_write_s("\n\r");
		}

	}
}
