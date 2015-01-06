/*
 * Sample skeleton source file.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "timer.h"
#include "serial.h"

void setup(){
	Serial.begin(9600);
	timer_init();
	
	//PORTB1 (AVR pin 15, Arduino pin D9) to output mode
	DDRB |= (1 << PORTB1);
	PORTB = ~PORTB;
	
	long long last_millis = timer_millis();
}	

void loop(){
  		_delay_ms(100);
		Serial.println("Abcdefg Hijkl");
		PORTB = ~PORTB;

/*		
		if (timer_millis() - last_millis > 1000){
			last_millis = timer_millis();
			PORTB = ~PORTB;
		}
*/
}

