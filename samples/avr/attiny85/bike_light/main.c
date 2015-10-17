#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

int main (void){
	DDRB |= _BV(PORTB3) | _BV(PORTB4);
	
	//Main program loop
	while (1){
		for (uint8_t i = 0; i < 5; i++){
			PORTB |= _BV(PORTB4);
			_delay_ms(50);
			PORTB &= ~_BV(PORTB4);
			_delay_ms(50);
		}
		for (uint8_t i = 0; i < 5; i++){
			PORTB |= _BV(PORTB3);
			_delay_ms(50);
			PORTB &= ~_BV(PORTB3);
			_delay_ms(50);
		}
		for (uint8_t i = 0; i < 5; i++){
			PORTB |= _BV(PORTB4);
			_delay_ms(50);
			PORTB &= ~_BV(PORTB4);
			_delay_ms(50);
		}
		for (uint8_t i = 0; i < 3; i++){
			PORTB |= _BV(PORTB4);
			_delay_ms(160);
			PORTB &= ~_BV(PORTB4);
			_delay_ms(160);
		}
	}
}

