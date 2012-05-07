#include "main.h"

int main() {
	
	shift_init(&PORT_SHIFT_LATCH, PIN_SHIFT_LATCH);
	button_init(&PORT_BUTTON_HOUR, PIN_BUTTON_HOUR, &PORT_BUTTON_MIN, PIN_BUTTON_MIN, &PORT_BUTTON_MODE, PIN_BUTTON_MODE);
	timer_init();
	sei();
		
	uint8_t segments[4];
	uint8_t red[8] = {0,0,0,0,0,0,0,0};
	uint8_t grn[8] = {0,0,0,0,0,0,0,0};
	
	int8_t life_count = 0;
	uint8_t life_reset = 0;
	uint8_t mode = 0;
	uint32_t prev_ms = 0;
	
	clock_mode(mode);
	life_randomize();

	while(1) {
		// get number of millis since midnight
		uint32_t ms = timer_millis();
		
		if (ms != prev_ms) {
			clock_update(ms);
			clock_segments(segments);
			display_set_segments(segments, clock_segment_flags());
			if (mode > 0) {
				clock_matrix(red, grn);
				display_set_matrix(red, grn);
			} else if (life_count++ == 0) {
				life_update();
				life_matrix(red, grn);
				display_set_matrix(red, grn);

				life_reset += 10;
				if (life_reset < 10) {
					life_randomize();
				}
			}
			button_sample();

			uint8_t changed = button_changed();
			uint8_t state = button_state();
			if ((changed & _BV(BUTTON_MODE)) && (state & _BV(BUTTON_MODE))) {
				mode++;
				if (mode >= CLOCK_MODES) mode = 0;
				clock_mode(mode);
			}
			if ((changed & _BV(BUTTON_HOUR)) && (state & _BV(BUTTON_HOUR))) {
				timer_add(clock_size_b());
			}
			if ((changed & _BV(BUTTON_MIN)) && (state & _BV(BUTTON_MIN))) {
				timer_add(clock_size_d());
			}
		}
				
		prev_ms = ms;
	}
}