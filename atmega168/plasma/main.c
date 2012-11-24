#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"
#include "lib/timer/timer.h"
#include "lib/twi/twi.h"

#include <util/delay.h>
#include <avr/wdt.h>

#define COUNT 1

int main (void){
	timer_init();
	uint8_t analog_pins[1] = {5};
	analog_init(analog_pins, 1, ANALOG_INTERNAL);
	twi_init();
	twi_set_master_buffer(matrix_get_working_buffer());
	
	wdt_enable(WDTO_500MS);

	srandom(analog_read_p(0) + timer_micros() + timer_millis());
	
	int8_t x[COUNT];
	int8_t y[COUNT];
	int8_t r[COUNT];
	int8_t xd[COUNT];
	int8_t yd[COUNT];
	uint8_t v[COUNT];
	
	for (uint8_t i = 0; i < COUNT; i++){
		x[i] = random() & 0x0F;
		y[i] = random() & 0x0F;
		r[i] = (random() & 0x7) + 0x7;
		xd[i] = ((random() & 0x1) ? 1 : -1) * ((random() & 0x1) ? 2 : 1);
		yd[i] = ((random() & 0x1) ? 1 : -1) * ((random() & 0x1) ? 2 : 1);
		v[i] = (i & 0x01) ? 0x0F : 0xF0;
	}

	while (1) {
		wdt_reset();
		srandom(analog_read_p(0) + timer_micros() + timer_millis());
	
		draw_rectangle(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, DRAW_FILLED, 0xFF, OVERLAY_NAND);
		
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
			draw_circle(x[i], y[i], r[i] - 0, DRAW_FILLED, v[i] & 0x11, OVERLAY_OR);
			draw_circle(x[i], y[i], r[i] - 1, DRAW_FILLED, v[i] & 0x22, OVERLAY_OR);
			draw_circle(x[i], y[i], r[i] - 2, DRAW_FILLED, v[i] & 0x33, OVERLAY_OR);
			draw_circle(x[i], y[i], r[i] - 4, DRAW_FILLED, v[i] & 0x44, OVERLAY_OR);
			draw_circle(x[i], y[i], r[i] - 5, DRAW_FILLED, v[i] & 0x88, OVERLAY_OR);
		}

		//Flush buffer
		twi_write_to(42, matrix_get_working_buffer(), TWI_BUFFER_LENGTH, TWI_BLOCK, TWI_STOP);
		
		_delay_ms(50);
	}
}
