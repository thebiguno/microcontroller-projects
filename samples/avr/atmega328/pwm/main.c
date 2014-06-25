#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[3];
	ports[0] = &PORTC;
	ports[1] = &PORTC;
	ports[2] = &PORTC;
	
	uint8_t pins[3];
	pins[0] = PORTC5;
	pins[1] = PORTC4;
	pins[2] = PORTC3;

	pwm_init(ports, pins, 3, 20000);
	pwm_set_phase(0, 100);
	pwm_set_phase(1, 1000);
	pwm_set_phase(2, 10000);

	//Main program loop
	DDRD = _BV(PORTD0);
	while (1){
	}
}
