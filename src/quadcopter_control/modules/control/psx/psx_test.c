/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../control_hardware.h"
#include "../../../lib/timer/timer.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/serial/serial.h"

char temp[32];

double pitch, last_pitch = 0;
double roll, last_roll = 0;
double yaw, last_yaw = 0;
double throttle, last_throttle = 0;


int main (void){
	//Do setup here

	serial_init(9600, 8, 0, 1);
	timer_init();

	init_control_hardware();

	//Main program loop
	while (1){

		_delay_ms(10);

		
		pitch = get_pitch();
		roll = get_roll();
		yaw = get_yaw();
		throttle = get_throttle();

		if (pitch != last_pitch || roll != last_roll || yaw != last_yaw || throttle != last_throttle){
			last_pitch = pitch;
			last_roll = roll;
			last_yaw = yaw;
			last_throttle = throttle;

			sprintf(temp, "%g", pitch);
			serial_write_s(temp);
			serial_write_s(", ");
			sprintf(temp, "%g", roll);
			serial_write_s(temp);
			serial_write_s(", ");
			sprintf(temp, "%g", yaw);
			serial_write_s(temp);
			serial_write_s(", ");
			sprintf(temp, "%g", throttle);
			serial_write_s(temp);
			serial_write_s("\n\r");
		}

	}
}
