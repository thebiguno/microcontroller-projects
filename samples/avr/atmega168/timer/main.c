/*
 * Sample skeleton source file.
 */


#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#include "lib/timer/timer.h"
#include "lib/serial/serial.h"

int main (void){
	//Do setup here
	serial_init(9600, 8, 0, 1);
	
	unsigned long long last_millis = timer_millis();	
	unsigned long long start_micros, end_micros;
	
	char s[20];
	
	timer_init();
	
	//Main program loop
	while (1){
		_delay_ms(1000);
		if (timer_millis() - last_millis >= 1000){
			last_millis = timer_millis();
			ltoa(last_millis, s, 10);
			
			start_micros = timer_micros();
			serial_write_s("abc");
			end_micros = timer_micros();
			
			serial_write_c(' ');
			serial_write_s(ltoa((end_micros - start_micros), s, 10));
			serial_write_c('\n');
			serial_write_c('\r');
		}
	}
}

