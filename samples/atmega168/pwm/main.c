#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[2];
	ports[0] = &PORTC;
	ports[1] = &PORTC;
	
	uint8_t pins[2];
	pins[0] = PORTC5;
	pins[1] = PORTC3;

	pwm_init(ports, pins, 2, 20000);
	pwm_set_phase(0, 100);
	pwm_set_phase(0, 1000);

	//Main program loop
	while (1){
		;
	}
}
