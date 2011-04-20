#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[1];
	ports[0] = &PORTD;
	
	uint8_t pins[1];
	pins[0] = PORTD7;

	uint16_t period = 2000;

	pwm_init(ports, pins, 1, period);
	
	DDRB |= _BV(PINB0);

	

	//Main program loop
	while (1){
		_delay_ms(100);
		
		period -= 25;
		pwm_set_period(period);
		pwm_set_phase(0, period / 2);
		if (period <= 100) period = 2000;

		
		PORTB = ~PORTB;
	}
}
