#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
 	volatile uint8_t *ports[1];
 	ports[0] = &PORTB;
// 	ports[1] = &PORTB;
// 	ports[2] = &PORTB;
// 	ports[3] = &PORTB;
// 	
 	uint8_t pins[1];
 	pins[0] = PORTB0;
// 	pins[1] = PORTB1;
// 	pins[2] = PORTB2;
// 	pins[3] = PORTB3;
// 
	PORTB |= _BV(PORTB5);
	DDRB |= _BV(PORTB5);
 	pwm_init(ports, pins, 1, 20000);
 	pwm_set_phase(0, 10);
// 	pwm_set_phase(1, 100);
// 	pwm_set_phase(2, 500);
// 	pwm_set_phase(3, 900);

	//Main program loop
	//DDRB |= _BV(PORTB4);
	while (1){
		_delay_ms(500);
		PORTB ^= _BV(PORTB4);
	}
}
