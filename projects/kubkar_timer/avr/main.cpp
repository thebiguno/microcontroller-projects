#include <avr/delay.h>

#include "lib/ShiftRegister/ShiftRegister.h"
#include "lib/Button/Buttons.h"
#include "lib/pwm/pwm.h"

using namespace digitalcave;

int main (void){
	ShiftRegister s(&PORTB, PORTB3, PORTB5);
	s.initLatch(&PORTB, PORTB2);
	s.initSpi();
	sei();
	
	uint8_t data[3];
	data[0] = 0x5A;
	data[1] = 0x5A;
	data[2] = 0x5A;
	
	DDRD = 0x0F;

	while (1) {
		for (uint8_t d = 0; d < 3; d++){	//Digit counter
			s.shift(data, 3);
			
			while (!s.cts());
			PORTD = ~_BV(d);
			//_delay_ms(100);
		}
	}
}
