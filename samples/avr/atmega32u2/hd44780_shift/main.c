#include <avr/io.h>
#include <util/delay.h>
#include "lib/Hd44780/Hd44780_Shift.h"

using namespace digitalcave;

int main (void){
	_delay_ms(100);
	DDRB |= _BV(PINB4);
	
	Hd44780_Shift display(&PORTB, 0, &PORTB, 4, &PORTB, 2, 1, display.FUNCTION_LINE_2 | display.FUNCTION_SIZE_5x8);

	uint8_t c1[8] = {31,31,0,0,0,0,31,31};
	uint8_t c2[8] = {0,0,0,0,0,0,31,31};
	uint8_t c3[8] = {31,31,0,0,0,0,0,0};
	display.setCgramAddress(0x00);
	_delay_ms(64);
	display.setBytes(c1, 8);
	display.setBytes(c2, 8);
	display.setBytes(c3, 8);

	display.setDdramAddress(0x00);
	_delay_ms(64);        //"1234567890123456789012345678901234567890"
	display.setByte(0xff);
	_delay_ms(64);
	display.setByte(0x00);
	_delay_ms(64);
	display.setByte(0x00);
	_delay_ms(64);
	display.setText((char *)"defghijklmnopqrstuvwxyz1234567890=-,.", 37);
	display.setDdramAddress(0x40);
	_delay_ms(64);
	display.setByte(0xff);
	_delay_ms(64);
	display.setByte(0x01);
	_delay_ms(64);
	display.setByte(0xff);
	_delay_ms(64);
	display.setText((char *)"DEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()+_<>", 37);
//	display.setText((char *)"\xba\xdd\xc6\xc1\xdc", 5);

	//Main program loop
	while (1){
		_delay_ms(0x1ff);
		display.setShift(display.SHIFT_SCREEN | display.SHIFT_LEFT);
	}
}

