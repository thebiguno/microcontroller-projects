#include "timer/timer.h"
#include "clock/clock.h"

#include "lib/analog/analog.h"
#include "lib/draw/draw.h"
#include "lib/draw/matrix/matrix.h"

int main() {
	matrix_init();
	matrix_set_mode(0x01);
	timer_init();
 	sei();
		
	uint32_t prev_ms = 0;
	
	while(1) {
		// get number of millis since midnight
		uint32_t ms = timer_millis();
		
		if (ms != prev_ms) {
			clock_draw(ms);
			matrix_write_buffer();
		}
		prev_ms = ms;
	}
}