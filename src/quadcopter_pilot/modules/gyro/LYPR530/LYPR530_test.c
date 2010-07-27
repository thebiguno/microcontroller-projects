/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../gyro.h"
#include "../../../lib/timer/timer.h"
#include "../../../lib/serial/serial.h"

char temp[32];

vector_t v, last_v;


int main (void){
	//Do setup here

	serial_init(9600, 8, 0, 1);
	timer_init();

	gyro_init();

	//Main program loop
	while (1){

		_delay_ms(10);

        v = gyro_get();

		if (v.x != last_v.x || v.y != last_v.y || v.z != last_v.z){
            last_v = v;

			serial_write_s(itoa(v.x, temp, 16));
			serial_write_s(", ");
			serial_write_s(itoa(v.y, temp, 16));
			serial_write_s(", ");
			serial_write_s(itoa(v.z, temp, 16));
			serial_write_s("\n\r");
		}

	}
}
