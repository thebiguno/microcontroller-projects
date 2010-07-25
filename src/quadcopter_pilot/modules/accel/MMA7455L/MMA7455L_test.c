/*
 * Sample skeleton source file.
 */


#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../accel.h"
//#include "../../../lib/timer/timer.h"
#include "../../../lib/serial/serial.h"

char temp[32];

vector_t v, last_v;

int16_t convert(double raw){
	return (int16_t) ((raw / 64.0) * 1000);
}

int main (void){
	//Do setup here

	serial_init(9600, 8, 0, 1);

	//timer_init();

	accel_init();
	accel_calibrate();

	//Main program loop
	while (1){
//		_delay_ms(10);

		int8_t data[3];
		v = accel_get(data);

		if (v.x != last_v.x || v.y != last_v.y || v.z != last_v.z){
			last_v = v;

			sprintf(temp, "x=%f, y=%f, z=%f\n\r", v.x, v.y, v.z);
			serial_write_s(temp);
		}
	}
}

