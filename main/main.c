#include "main.h"

int main() {
	
	shift_init(&PORT_SHIFT_DATA, PIN_SHIFT_DATA, &PORT_SHIFT_CLOCK, PIN_SHIFT_CLOCK, &PORT_SHIFT_LATCH, PIN_SHIFT_LATCH);
	button_init(&PORT_BUTTON_HOUR, PIN_BUTTON_HOUR, &PORT_BUTTON_MIN, PIN_BUTTON_MIN, &PORT_BUTTON_MODE, PIN_BUTTON_MODE);
	timer_init();
	
	// 	uint8_t MATRIX_BOX[8] = { 0xFF,0x81,0x81,0x81,0x81,0x81,0x81,0xFF };
	// 	uint8_t MATRIX_X[8] = { 0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81 };
	// 	uint8_t MATRIX_CTR[8] = { 0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00 };
	// 	uint8_t MATRIX_GRN[8] = { 0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA };
	// 	uint8_t MATRIX_RED[8] = { 0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55 };
	// 	uint8_t MATRIX_OFF[8] = { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
	// 	uint8_t MATRIX_ON[8] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };

	char c[] = "    ";
	uint8_t red[8] = {0,0,0,0,0,0,0,0};
	uint8_t grn[8] = {0,0,0,0,0,0,0,0};

	uint32_t prev_ms = 0;
	uint8_t mode = 0;
	uint8_t flag = 0;
	
	clock_mode(mode);
	char str[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890";
	scroll_update(str, sizeof(str) - 1);
	
	while(1) {
		// get number of millis since midnight
		uint32_t ms = timer_millis();
		
		// sample buttons
		if (prev_ms != ms) { // limit button sampling to 1 ms
			prev_ms = ms;
			button_sample();
		}
		
		// do something on button press
		uint8_t changed = button_changed();
		uint8_t state = button_state();
		if ((changed & _BV(BUTTON_MODE)) && (state & _BV(BUTTON_MODE))) {
			mode++;
			if (mode > 4) mode = 0;
			clock_mode(mode);
		}
		if ((changed & _BV(BUTTON_HOUR)) && (state & _BV(BUTTON_HOUR))) {
			timer_add(clock_size_b());
		}
		if ((changed & _BV(BUTTON_MIN)) && (state & _BV(BUTTON_MIN))) {
			timer_add(clock_size_d());
		}
		
		// update the clock segments
		if ((uint8_t) ms == 0) { // every 256 ms
			if (flag == 0) {
				flag = 1;
				clock_update(ms);
				clock_segments(c);
				clock_matrix(red, grn);
				//scroll_draw(red, grn);
			}
		} else {
			flag = 0;
		}

		segment_draw(c, 0x00);
		matrix_draw(red,grn);
		shift_latch();
	}
}