#include "lib/draw/draw.h"
#include "lib/analog/analog.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/serial/serial.h"
#include "lib/timer/timer.h"
#include "lib/draw/fonts/tall.h"
#include "lib/Shift/Shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>


int main (void){
	uint8_t analog_pins[6] = {0, 1, 2, 3, 4, 5};
	Shift shift(&PORTB, PORTB0, &PORTB, PORTB1);

	timer_init();
	analog_init(analog_pins, 6, ANALOG_INTERNAL);
	draw_rectangle(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, DRAW_FILLED, RED_3, OVERLAY_OR);

	uint8_t sync_frame = 0x42;

	_delay_ms(50);

	while (1) {
//		shift.send(&sync_frame, 1);
		shift.send(matrix_get_working_buffer(), MATRIX_WIDTH * (MATRIX_HEIGHT >> 1));
		
		_delay_ms(10);
	}
}
