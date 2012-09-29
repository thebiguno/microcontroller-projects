#include "lib/shiftcpp/shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include "lib/serial/serial.h"

static Shift shift(13);
uint8_t data[13];

int main (void){
	data[0] = 0xFF;
	data[1] = 0xFF;
	data[2] = 0xFF;
	data[3] = 0xFF;
	data[4] = 0xFF;
	data[5] = 0xFF;
	data[6] = 0xFF;
	data[7] = 0xFF;
	data[8] = 0xFF;
	data[9] = 0xFF;
	data[10] = 0xFF;
	data[11] = 0xFF;

	uint8_t row = 0;
	shift.setLatch(&PORTB, PORTB2);
	while (1) {
		if (shift.cts()) {
			data[12] = ~_BV(row++);
			if (row > 7) row = 0;
			shift.shift(data);
		}
	}
}
