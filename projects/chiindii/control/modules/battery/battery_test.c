#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "../../lib/serial1/serial1.h"
#include "battery.h"


int main (void){

	battery_init();	
	serial1_init_b(9600);
	
	char temp[32];
	
	while(1){
		sprintf(temp, "%f\n\r", battery_level());
		serial1_write_s(temp);
		_delay_ms(1000);
	}
}
