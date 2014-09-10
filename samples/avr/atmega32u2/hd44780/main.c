#include <avr/io.h>
#include <util/delay.h>
#include "lib/Hd44780/Hd44780.h"

using namespace digitalcave;

int main (void){
	_delay_ms(100);
	DDRB |= _BV(PINB4);
	
	Hd44780 display(&PORTB, 0, &PORTB, 2, 1, display.FUNCTION_LINE_2 | display.FUNCTION_SIZE_5x8);

	display.setDdramAddress(0x00);
	_delay_ms(64);
	display.setText((char *)"Hello World", 11);
	display.setDdramAddress(0x40);
	_delay_ms(64);
	display.setText((char *)"\xba\xdd\xc6\xc1\xdc", 5);

	//Main program loop
	
//	SPDR = 0x5a;
//	while(!(SPSR & (1<<SPIF)));
//	PORTB &= ~_BV(PB2);

	while (1){
	}
}

