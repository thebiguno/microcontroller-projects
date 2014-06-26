#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[4];
	ports[0] = &PORTD;
	ports[1] = &PORTC;
	ports[2] = &PORTC;
	ports[3] = &PORTC;
	
	uint8_t pins[4];
	pins[0] = PORTD1;
	pins[1] = PORTC5;
	pins[2] = PORTC3;
	pins[3] = PORTC1;

	pwm_init(ports, pins, 4, 20000);

	//Main program loop
	DDRD |= _BV(PORTD0);
	while (1){

		pwm_set_phase_batch(0, 500);
		pwm_set_phase_batch(1, 1000);
		pwm_set_phase_batch(2, 2000);
		pwm_set_phase_batch(3, 3000);
		pwm_apply_batch();

		_delay_ms(100);
	}
}
