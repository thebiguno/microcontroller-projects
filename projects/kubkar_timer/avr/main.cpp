#include <avr/delay.h>

#include "lib/ShiftRegister/ShiftRegister.h"
#include "lib/Button/Buttons.h"
#include "lib/pwm/pwm.h"

using namespace digitalcave;

int main (void){
	DDRB |= _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB5);
	//SPCR |= _BV(SPI2X);
	SPCR = _BV(SPE) | _BV(MSTR);

	uint8_t data[3];
	data[0] = ~0x3E;
	data[1] = ~0x3E;
	data[2] = ~0x3E;
	
	DDRD = 0x0F;

	while (1) {
		for (uint8_t d = 0; d < 4; d++){	//Digit counter
			for (uint8_t b = 0; b < 3; b++){
				SPDR = data[b] & ~d;
				while (!(SPSR & _BV(SPIF)));
			}
			PORTD = 0x00;
			PORTB &= ~_BV(PORTB2);
			PORTB |= _BV(PORTB2);
			PORTD = ~_BV(d);
			_delay_ms(2);
		}
	}
}
