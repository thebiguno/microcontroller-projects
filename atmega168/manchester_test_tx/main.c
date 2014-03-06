#include <util/delay.h>
#include "../../lib/manchester/manchester.h"

int main (){
	DDRD = 0xFF;	//TODO
	PORTD = 0x00;	//TODO
	_delay_ms(1000);
	
	char data = 0x00;
	
	manchester_init_tx(&PORTB, PORTB0, 300);
	
	while (1){
		for (uint8_t i = 0; i < 0x10; i++){
			manchester_write(data++);
		}
		_delay_ms(1000);
	}   
}
