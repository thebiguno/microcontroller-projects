#include <util/delay.h>
#include "../../lib/manchester/manchester.h"

int main (){
	manchester_init_rx(300);
		
	//char s;
	
	uint8_t data;
	
	manchester_write(0x00);
	
	while (1){
		manchester_read(&data);
		
		_delay_ms(1000);
	}   
}
