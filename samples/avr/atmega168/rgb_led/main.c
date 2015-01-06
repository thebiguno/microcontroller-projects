#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	volatile uint8_t *ports[3];
	ports[0] = &PORTB;
	ports[1] = &PORTB;
	ports[2] = &PORTB;
	
	uint8_t pins[3];
	pins[0] = PORTB1;
	pins[1] = PORTB2;
	pins[2] = PORTB3;

	pwm_init(ports, pins, 3, 2560);	

	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;

	//Main program loop
	while (1){
		_delay_ms(100);
		
		pwm_set_phase(0, red * 10);
//		pwm_set_phase(1, green * 10);
//		pwm_set_phase(2, blue * 10);
		
		red++;
		green++;
		blue++;
	}
}
