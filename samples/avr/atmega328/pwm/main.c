#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[3];
	ports[0] = &PORTD;
	ports[1] = &PORTC;
	ports[2] = &PORTC;
	
	uint8_t pins[3];
	pins[0] = PORTD2;
	pins[1] = PORTC4;
	pins[2] = PORTC3;

	pwm_init(ports, pins, 3, 20000);
	pwm_set_phase_batch(0, 2000);
	pwm_set_phase_batch(1, 1000);
	pwm_set_phase_batch(2, 10000);
	pwm_apply_batch();

	//Main program loop
	DDRD = _BV(PORTD0);
	while (1){
	}
}
