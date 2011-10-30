#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
/*
	uint16_t l0 = 100, l1 = 1000;
	uint16_t s0, s1;	//Step; will be assigned randomly

	//Do setup here
	volatile uint8_t *ports[2];
	ports[0] = &PORTB;
	ports[1] = &PORTB;
	
	uint8_t pins[2];
	pins[0] = PORTB2;	//Yellow
	pins[1] = PORTB1;	//Red

	pwm_init(ports, pins, 2, 2000);
	pwm_set_phase(0, l0);
	pwm_set_phase(1, l1);

	//Main program loop
	while (1){
		s0 = random() & 0xFFF;
		s1 = random() & 0xFFFFF;	//More flicker in red
	
		if (s0 & 0x1) l0 += s0;
		else l0 -= s0;
		
		if (s1 & 0x1) l1 += s1;
		else l1 -= s1;

		pwm_set_phase(0, l0);
		pwm_set_phase(1, l1);
		_delay_ms(100);
	}
*/

	DDRB |= _BV(PORTB1) | _BV(PORTB2);
	
	uint16_t r;
	while(1){
		r = random();
		if (r & 0x1) PORTB |= _BV(PORTB1);
		else PORTB &= ~_BV(PORTB1);
		
		if (r & 0x11) PORTB |= _BV(PORTB2);
		else PORTB &= ~_BV(PORTB2);
	}
}

