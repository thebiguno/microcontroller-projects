#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[1];
	ports[0] = &PORTC;
	
	uint8_t pins[4];
	pins[0] = PORTC5;

	pwm_init(ports, pins, 1, 512);
	pwm_set_phase(0, 5);

	//Main program loop
	while (1){
		pwm_set_phase(0, 1);
		_delay_ms(1000);
		pwm_set_phase(0, 0);
		_delay_ms(1000);
	}
}
