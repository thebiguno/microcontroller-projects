#include <avr/io.h>
#include <avr/power.h>
#include <util/delay.h>

#include "Chiindii.h"

int main(){
	//Set clock to run at full speed
	clock_prescale_set(clock_div_1);
	// DISABLE JTAG - take control of F port
	MCUCR = _BV(JTD);
	MCUCR = _BV(JTD);
	
	DDRF |= _BV(5) | _BV(6) | _BV(7);
	DDRB |= _BV(0) | _BV(1);
	
	//Set motor pins as output
	*(&MOTOR1_PORT - 0x01) |= _BV(MOTOR1_PIN);
	*(&MOTOR2_PORT - 0x01) |= _BV(MOTOR2_PIN);
	*(&MOTOR3_PORT - 0x01) |= _BV(MOTOR3_PIN);
	*(&MOTOR4_PORT - 0x01) |= _BV(MOTOR4_PIN);
	
	//Main program loop
	while (1) {
		MOTOR1_PORT ^= _BV(MOTOR1_PIN);
		
		PORTB ^= _BV(0) | _BV(1);
		PORTF ^= _BV(5) | _BV(6) | _BV(7);
		_delay_ms(1000);
	}
}
