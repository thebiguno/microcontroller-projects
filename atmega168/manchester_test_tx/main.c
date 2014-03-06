#include <util/delay.h>
#include "../../lib/manchester/manchester.h"

int main (){
	DDRD = 0xFF;	//TODO
	PORTD = 0x00;	//TODO
	_delay_ms(1000);
	
	char data = 0x00;
	
	manchester_init_tx(&PORTB, PORTB0, 1200);
	
	while (1){
		manchester_write(data++);
	}   
}
