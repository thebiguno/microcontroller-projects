#include <avr/io.h>
#include "lib/pwm/pwm.h"

// these values represent a typical ESC that runs at 50Hz, 
// turns motor off at a pulse width of 1000ms, 
// and runs at 100% duty-cycle at a pluse width of 2000ms
#define ESC_M 1000
#define ESC_B 1000
#define PULSE_SPACING 20000 // 20 ms - 50 Hz

void esc_init() {
	volatile uint8_t *ports[8];
	ports[0] = &PORTB;
	ports[1] = &PORTB;
	ports[2] = &PORTB;
	
	uint8_t pins[8];
	pins[0] = 2;
	pins[1] = 1;
	pins[2] = 3;
	
	pwm_init(ports, pins, 3, PULSE_SPACING);
    pwm_start();
}

uint16_t _scale(double x) {
    return ESC_M * x + ESC_B;
}

void esc_set(double speed[]) {
    for (int i = 0; i < 4; i++) {
        pwm_set_phase(i, _scale(speed[i]));
    }
}
