#include "status.h"

void status_set_color(uint8_t red, uint8_t green, uint8_t blue){
	pwm_set_phase_batch(LED_RED, red * 78);		//The magic number 78 comes from X/20000 = color/255; X = color * 20000 / 255; X = color * 78.
	pwm_set_phase_batch(LED_GREEN, green * 78);
	pwm_set_phase_batch(LED_BLUE, blue * 78);
	
	pwm_apply_batch();
}