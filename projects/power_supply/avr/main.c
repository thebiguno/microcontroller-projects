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

	volatile uint8_t *ports[1];
	ports[0] = &PORTB;
	
	uint8_t pins[1];
	pins[0] = PORTB6;

	pwm_init(ports, pins, 1, 2048);

	
	//Main program loop
	while (1){
	}
}

ISR(PCINT0_vect){
	static uint8_t red = 0x00;
	static uint8_t last_enc1 = 0x00;
	uint8_t enc1 = PINB & 0x03;
	
	last_enc1 = ((last_enc1 << 2) | enc1) & 0x0F;
	
	switch(last_enc1){
		case 0x02:
		case 0x08:
		case 0x0B:
		case 0x0D:
			red--;
			pwm_set_phase(0, (uint16_t) red * 8);
			break;
			
		default:
			red++;
			pwm_set_phase(0, (uint16_t) red * 8);
	}
}