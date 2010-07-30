#include <avr/io.h>
#include <util/delay.h>
#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	uint8_t value = 0x00;	

	//Main program loop
	while (1){
		pwm_set(&PORTD, &DDRD, PORTD6, value++);
		_delay_ms(100);
		if (value > 0xFF) value = 0x00;
	}
}
