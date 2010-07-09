/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../control-hardware.h"
#include "../../../lib/timer/timer.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/serial/serial.h"

char temp[32];

int16_t pitch, last_pitch = 0;
int16_t roll, last_roll = 0;
int16_t yaw, last_yaw = 0;
int16_t throttle, last_throttle = 0;


int main (void){
	//Do setup here

	serial_init(9600, 8, 0, 1);
	timer_init();

	init_hardware();

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

			serial_write_s(itoa(pitch, temp, 10));
			serial_write_s(", ");
			serial_write_s(itoa(roll, temp, 10));
			serial_write_s(", ");
			serial_write_s(itoa(yaw, temp, 10));
			serial_write_s(", ");
			serial_write_s(itoa(throttle, temp, 10));
			serial_write_s("\n");
		}

	}
}
