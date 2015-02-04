#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[1];
	ports[0] = &PORTC;
	
	uint8_t pins[4];
	pins[0] = PORTC6;

	pwm_init(ports, pins, 1, 512);

	uint8_t x = 0;
	//Main program loop
	while (1){
		pwm_set_phase(0, x * 2);
		x+= 8;
		_delay_ms(100);
	}
}
