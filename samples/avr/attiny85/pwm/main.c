#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	volatile uint8_t ports[1];
	ports[0] = &PORTB;
	
	uint8_t pins[1];
	pins[0] = 0;

	//Do setup here
	pwm_init(ports, pins, 1, 1000);
	
	uint8_t value = 0x00;

	//Main program loop
	while (1){
		pwm_set(&PORTD, &DDRD, PORTD6, value++);
		_delay_ms(100);
		if (value > 0xFF) value = 0x00;
	}
}
