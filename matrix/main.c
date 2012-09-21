#include "lib/shiftcpp/shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "lib/serial/serial.h"

static Shift shift(1);
uint8_t data[1];

int main (void){
	data[0] = 0x01;
	serial_init_b(57600);
	shift.setLatch(&PORTB, PORTB2);
	while (1) {
		serial_write_s("loop\n\r");
		data[0] *= 2;
		if (data[0] == 0) data[0] = 0x01;
		if (shift.cts()) {
			shift.shift(data);
			serial_write_s("shift\n\r");
		}
		_delay_ms(1000);
	}
}
