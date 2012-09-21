#include "lib/shiftcpp/shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "lib/serial/serial.h"

static Shift shift(2);
uint8_t data[2];

int main (void){
	data[0] = 0xFF;
	uint8_t row = 0;
	serial_init_b(57600);
	shift.setLatch(&PORTB, PORTB2);
	while (1) {
		serial_write_s("loop\n\r");
		if (row > 7) row = 0;
		data[1] = _BV(row);
		if (shift.cts()) {
			shift.shift(data);
			serial_write_s("shift\n\r");
		}
		_delay_ms(1000);
	}
}
