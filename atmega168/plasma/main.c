#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/timer/timer.h"
#include "lib/twi/twi.h"
#include <util/delay.h>

#define COUNT 3

int main (void){
	timer_init();
	uint8_t analog_pins[1] = {5};
	analog_init(analog_pins, 1, ANALOG_INTERNAL);
	twi_init();
	twi_set_master_buffer(matrix_get_working_buffer());

	srandom(analog_read_p(0) + timer_micros() + timer_millis());
	
	int8_t x[COUNT];
	int8_t y[COUNT];
	int8_t r[COUNT];
	int8_t xd[COUNT];
	int8_t yd[COUNT];
	uint8_t v[COUNT];
	uint8_t o[COUNT];
	
	v[0] = GRN_3;
	v[1] = RED_3;
	v[2] = GRN_1;
//	v[3] = GRN_3;
//	v[4] = RED_3;
	
	o[0] = OVERLAY_OR;
	o[1] = OVERLAY_OR;
	o[2] = OVERLAY_OR;
//	o[3] = OVERLAY_XOR;
//	o[4] = OVERLAY_XOR;
	
	for (uint8_t i = 0; i < COUNT; i++){
		x[i] = random() & 0x0F;
		y[i] = random() & 0x0F;
		r[i] = (random() & 0x7) + 0x7;
		xd[i] = ((random() & 0x1) ? 1 : -1) * ((random() & 0x1) ? 2 : 1);
		yd[i] = ((random() & 0x1) ? 1 : -1) * ((random() & 0x1) ? 2 : 1);
	}

	while (1) {
		srandom(analog_read_p(0) + timer_micros() + timer_millis());
	
		draw_rectangle(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, DRAW_FILLED, 0xF, OVERLAY_NAND);
		
		for (uint8_t i = 0; i < COUNT; i++){
			//Bounds checking
			if (x[i] < 0) x[i] = 0;
			if (y[i] < 0) y[i] = 0;
			
			//Adjust size of circles
			if ((random() & 0xF) == 0xF) r[i] += ((random() & 0x1) ? 1 : -1);
			if (r[i] > 0xF) r[i] = 0xF;
			if (r[i] < 0x7) r[i] = 0x7;
			
			//Move circles		
			x[i] += xd[i];
			y[i] += yd[i];
			
			//Bounce
			if (x[i] <= 0 || x[i] >= MATRIX_WIDTH) xd[i] *= -1;
			if (y[i] <= 0 || y[i] >= MATRIX_WIDTH) yd[i] *= -1;
	
			//Draw circle
			draw_circle(x[i], y[i], r[i], DRAW_FILLED, v[i] & 0x11, o[i]);
			draw_circle(x[i], y[i], r[i] - 3, DRAW_FILLED, v[i] & 0x11, (o[i] == OVERLAY_OR ? OVERLAY_NAND : OVERLAY_OR));
			draw_circle(x[i], y[i], r[i] - 3, DRAW_FILLED, v[i] & 0x22, o[i]);
			draw_circle(x[i], y[i], r[i] - 6, DRAW_FILLED, v[i], o[i]);		
		}

		//Flush buffer
		twi_write_to(42, matrix_get_working_buffer(), 192, TWI_BLOCK, TWI_STOP);
		
		_delay_ms(50);
	}
}
