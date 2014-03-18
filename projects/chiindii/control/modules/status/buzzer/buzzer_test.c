#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "../status.h"


int main (void){

	status_error_init();
	uint8_t error = 0;
	
	while(1){
		status_error_battery(error);
		error = ~error;
		
		_delay_ms(1000);
	}
}
