#include "main.h"

#include "segment/segment.h"

// projector switch on; project advance; camera switch on; camera shutter;
#define T0 255
#define T1 2048
#define T2 255
#define T3 512


int main() {
	
	shift_init(&PORT_SHIFT_DATA, PIN_SHIFT_DATA, &PORT_SHIFT_CLOCK, PIN_SHIFT_CLOCK, &PORT_SHIFT_LATCH, PIN_SHIFT_LATCH);
	button_init(&PORT_BUTTON_PLAY_PAUSE, PIN_BUTTON_PLAY_PAUSE, &PORT_BUTTON_SLIDE_COUNT, PIN_BUTTON_SLIDE_COUNT);
	timer_init();
	sei();
		
	uint8_t segments[4];
	
	uint8_t step = 0;
	uint16_t delay = 0;
	uint8_t play = 0; // 0 = pause; ~0 = play
	uint8_t slide_count = 80; // counts down from 36, 80, or 140 to 0
	uint32_t prev_ms = 0;
	
	while(1) {
		uint32_t ms = timer_millis();
		
		if (ms != prev_ms) {
			if (play) {
				segments[0] = segment_character('^');

				if (step == 0) {
					if (delay == 0) {
						step++;
						delay = T1;
					}
				} else if (step == 1) {
					if (delay == 0) {
						step++;
						delay = T2;
					}
				} else if (step == 2) {
					if (delay == 0) {
						step++;
						delay = T3;
					}
				} else {
					if (delay == 0) {
						slide_count--;
						step = 0;
						delay = T0;

						uint8_t temp = slide_count;
						uint8_t a = 0;
						uint8_t b = 0;
						uint8_t c = 0;
						while (temp > 99) {
							a += 1;
							temp -= 100;
						}
						while (temp > 9) {
							b += 1;
							temp -= 10;
						}
						c = temp;

						if (a == 0) {
							segments[1] = segment_character(' ');
							if (b == 0) {
								segments[2] = segment_character(' ');
							}
						}
						segments[1] = segment_decimal(a);
						segments[2] = segment_decimal(b);
						segments[3] = segment_decimal(c);
					}
				}
			} else {
				segments[0] = segment_character('_');
			}
			
			button_sample();

			uint8_t changed = button_changed();
			uint8_t state = button_state();
			if ((changed & _BV(BUTTON_PLAY_PAUSE)) && (state & _BV(BUTTON_PLAY_PAUSE))) {
				play = ~play;
			}
			if ((changed & _BV(BUTTON_SLIDE_COUNT)) && (state & _BV(BUTTON_SLIDE_COUNT))) {
				play = 0;
				
				if (slide_count < 36) {
					slide_count = 36;
				} else if (slide_count < 80) {
					slide_count = 80;
				} else if (slide_count < 144) {
					slide_count = 144;
				} else {
					slide_count = 36;
				}
			}
		}
				
		prev_ms = ms;
	}
}