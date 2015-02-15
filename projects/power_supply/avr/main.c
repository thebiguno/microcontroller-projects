#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "lib/pwm/pwm.h"

int main (void){
	//Do setup here
	PCICR |= _BV(PCIE0);
	PCMSK0 |= 0x0F;	//Enable bits 0..3 for pin change interrupts
	PORTB |= _BV(PORTB0) | _BV(PORTB1);
	sei();

	DDRB |= _BV(PORTB5) | _BV(PORTB6);
	
	//Main program loop
	while (1){
	}
}

ISR(PCINT0_vect){
	static uint8_t last_enc1 = 0x00;
	uint8_t enc1 = PINB & 0x03;
	
	last_enc1 = ((last_enc1 << 2) | enc1) & 0x0F;
	
	switch(last_enc1){
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			PORTB ^= _BV(PORTB6);
			break;
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			PORTB ^= _BV(PORTB5);
			break;
	}

}