#include <avr/io.h>
#include <util/delay.h>
#include "lib/Hd44780/Hd44780_Direct.h"

using namespace digitalcave;

int main (void){
	_delay_ms(100);
	
	Hd44780_Direct display(display.FUNCTION_LINE_2 | display.FUNCTION_SIZE_5x8, &PORTE, 6, &PORTE, 2, &PORTB, 7, &PORTD, 5, &PORTC, 6, &PORTC, 7);

	uint8_t c1[8] = {31,31,0,0,0,0,31,31};
	uint8_t c2[8] = {0,0,0,0,0,0,31,31};
	uint8_t c3[8] = {31,31,0,0,0,0,0,0};
	display.set_cgram_address(0x00);
	display.write_bytes(c1, 8);
	display.write_bytes(c2, 8);
	display.write_bytes(c3, 8);

	display.set_ddram_address(0x00);
	for(uint8_t i = 0x30; i < 0x30 + 20; i++){
		display.write_byte(i);
	}
	for(uint8_t i = 0x40; i < 0x40 + 20; i++){
		display.write_byte(i);
	}
	for(uint8_t i = 0x50; i < 0x50 + 20; i++){
		display.write_byte(i);
	}
	for(uint8_t i = 0x60; i < 0x60 + 20; i++){
		display.write_byte(i);
	}

	//Main program loop
	while (1){
		//_delay_ms(2000);
		//display.set_shift(display.SHIFT_SCREEN | display.SHIFT_LEFT);
	}
}

