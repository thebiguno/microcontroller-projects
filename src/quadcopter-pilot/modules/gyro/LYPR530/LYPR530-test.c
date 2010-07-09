/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../gyro-hardware.h"
#include "../../../lib/timer/timer.h"
#include "../../../lib/serial/serial.h"

char temp[32];

int16_t x, last_x = 0;
int16_t y, last_y = 0;
int16_t z, last_z = 0;


int main (void){
	//Do setup here

	serial_init(9600, 8, 0, 1);
	timer_init();

	init_gyro_hardware();

	//Main program loop
	while (1){

		_delay_ms(10);

		
		x = get_gyro_x();
		y = get_gyro_y();
		z = get_gyro_z();

		if (x != last_x || y != last_y || z != last_z){
			last_x = x;
			last_y = y;
			last_z = z;

			serial_write_s(itoa(x, temp, 16));
			serial_write_s(", ");
			serial_write_s(itoa(y, temp, 16));
			serial_write_s(", ");
			serial_write_s(itoa(z, temp, 16));
			serial_write_s("\n\r");
		}

	}
}
