#include <avr/io.h>
#include <util/delay.h>
#include "../esc.h"
#include "lib/pwm/pwm.h"



int main (void){
	//Do setup here
	esc_init();

	uint16_t value = 100;
	//Main program loop
	while (1){
		pwm_set(0, value);
		value += 100;
		_delay_ms(1000);
	}
}
