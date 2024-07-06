#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include <analog.h>

#include "pwm.h"
#include "serial.h"

int main (void){
	uint8_t analogPins[1];
	analogPins[0] = 5;
	analog_init(analogPins, 1, ANALOG_AVCC);
	

	
	volatile uint8_t *pwmPorts[1];
	pwmPorts[0] = &PORTD;
	
	uint8_t pwmPins[1];
	pwmPins[0] = PORTD3;

	pwm_init(pwmPorts, pwmPins, 1, 10240);
//	pwm_set_phase(0, 1000);

//	pwm_start();

	serial_init_b(57600);
	
	char temp[32];
	
	//Main program loop
	while (1){
		uint32_t potentiometer = analog_read_p(0);
		
		if (potentiometer < 10){
			potentiometer = 0;
		}
		
		pwm_set_phase(0, potentiometer * 10);

		sprintf(temp, "%d\n\r", potentiometer);
		serial_write_s(temp);
		
		_delay_ms(100);
	}
}
