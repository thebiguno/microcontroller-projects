#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/serial/serial.h"
#include "lib/TwoWire/TwoWire.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>


int main (void){
	draw_rectangle(0, 0, MATRIX_WIDTH - 1, MATRIX_HEIGHT - 1, DRAW_FILLED, RED_3, OVERLAY_OR);

	_delay_ms(1000);

	Wire.begin();
	
	while (1) {
		Wire.beginTransmission(42);
		Wire.send(matrix_get_working_buffer(), MATRIX_WIDTH * (MATRIX_HEIGHT >> 1));
		Wire.endTransmission();
		
		_delay_ms(10);
	}
}
