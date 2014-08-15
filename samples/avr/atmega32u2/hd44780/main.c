#include <avr/io.h>
#include <util/delay.h>
#include "lib/Hd44780/Hd44780.h"

using namespace digitalcave;

int main (void){
	Hd44780 display(&PORTB, 0, &PORTB, 2, 1);
	display.init();
	display.setFunction(0x0e);	// 5x8, 2-line, 8-bit
	_delay_us(37);
	display.setMode(0x02);		// no shift, increment
	_delay_us(37);
	display.clear();
	_delay_ms(1.52);
	
	display.setDdramAddress(0x00);
	_delay_ms(37);
	display.setText("Hello World", 11);
	display.setDdramAddress(0x40);
	_delay_ms(37);
	display.setText("Ol\xe1 Mundo", 9);

	//Main program loop
	while (1){
		;
	}
}

