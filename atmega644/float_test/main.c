/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "lib/serial/serial.h"

union udouble {
	double d;
	uint8_t u[sizeof(double)];
};

void _double_to_bytes(double value, uint8_t *buffer) {
    union udouble converter;
    converter.d = value;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        buffer[i] = converter.u[i];
    }
}
double _bytes_to_double(uint8_t *buffer) {
    union udouble converter;
    for (uint8_t i = 0; i < sizeof(double); i++) {
        converter.u[i] = buffer[0];
    }
    return converter.d;
}

int main (void){
	serial_init(57600, 8, 2, 1);
		
	// DDRB |= _BV(PINB0) | _BV(PINB1) | _BV(PINB4);

	uint8_t buf[4];

	_delay_ms(1000);

	//Main program loop

	float f[] = { 0.0, -0.0, 1.0, -1.0, INFINITY, -INFINITY, NAN, 3.14159, -3.14159 };
	for (int i = 0; i < 9; i++) {
		_double_to_bytes(f[i], buf);
		for (int i = 0; i < 4; i++) {
			serial_write_c(buf[i]);
			_delay_ms(10);
		}
	}
	for (float f = -3.141592653589793; f <= 3.141592653589793; f = f + 0.017453292519943) {
			// PORTB ^= _BV(PINB4);
		
			_double_to_bytes(f, buf);
			// PORTB |= _BV(PINB0);
			for (int i = 0; i < 4; i++) {
				serial_write_c(buf[i]);
			}
			// PORTB &= ~(_BV(PINB0) | _BV(PINB1));
			
			_delay_ms(10);
		}
	while(1) {
	}	
}

