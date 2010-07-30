#include <avr/io.h>
#include "lib/pwm/pwm.h"

void esc_init() {
	volatile uint8_t *ports[8];
	ports[0] = &PORTB;
	ports[1] = &PORTB;
	ports[2] = &PORTB;
	
	uint8_t pins[8];
	pins[0] = 2;
	pins[1] = 1;
	pins[2] = 3;
	
	pwm_init(ports, pins, 3, 20000);
}

/*
 * 
 */
void esc_set(uint8_t armed, double speed[]) {
	/*
    if (armed) {
        // TODO
        for (int i = 0; i < 4; i++) {
            _esc_set(i, scale(speed[i]));
        }
    } else {
        for (int i = 0; i < 4; i++) {
            _esc_set(i, 0);
        }
    }
    */
}
