/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "protocol.h"
#include "lib/serial/serial.h"

int main (void){
	serial_init_b(57600);
	
	_delay_ms(3000);
		
	uint8_t buf[36];

	double x[] = { 0.0, -0.0, 1.0, -1.0, INFINITY, -INFINITY, NAN, 3.14159, -3.14159 };
	int i;
	for (i = 0; i < 9; i++) {
		protocol_double_to_bytes(x[i], buf, i * 4);
	}
	protocol_send_message('X', buf, 36);
	
	double d;
	for (d = -3.141592653589793; d <= 3.141592653589793; d = d + 0.349065850398866) {
		protocol_double_to_bytes(d, buf, i * 4);
	}
	protocol_send_message('R', buf, 36);
	
	while(1) {
	}	
}

