#include <util/delay.h>
#include "../../lib/manchester/manchester.h"

int main (){
	DDRD = 0xFF;	//TODO
	PORTD = 0x00;	//TODO
	_delay_ms(1000);
	PORTD |= _BV(PORTD2);	//TODO
	_delay_ms(5);
	PORTD &= ~_BV(PORTD2);	//TODO
	
	manchester_init_tx(&PORTB, PORTB0, 300);
	PORTD |= _BV(PORTD3);	//TODO
	
	while (1){
		manchester_write(0xA5);
		
		_delay_ms(1000);
	}   
}
