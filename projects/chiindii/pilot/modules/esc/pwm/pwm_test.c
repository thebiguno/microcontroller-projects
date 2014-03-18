#include <avr/io.h>
#include <util/delay.h>
#include "../esc.h"
#include "../../../lib/pwm/pwm.h"

int main (void){
	//Do setup here
	esc_init();

	pwm_set_phase(0, 100);
	pwm_set_phase(1, 500);
	pwm_set_phase(2, 5500);
	
	uint32_t period = 20000;
	
	//Main program loop
	while (1){
		_delay_ms(1000);
		pwm_stop();
		_delay_ms(1000);
		pwm_start();
	}
}
