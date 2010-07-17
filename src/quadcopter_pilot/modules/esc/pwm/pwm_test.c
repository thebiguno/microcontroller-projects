#include <avr/io.h>
#include <util/delay.h>
#include "../esc.h"
#include "lib/pwm/pwm.h"



int main (void){
	//Do setup here
	esc_init();

	pwm_set(0, 100);
	pwm_set(1, 500);
	pwm_set(2, 1500);
	
	//Main program loop
	while (1){
	}
}
