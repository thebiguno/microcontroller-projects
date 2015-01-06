#include <util/delay.h>
#include <avr/wdt.h>
#include "../../lib/manchester/manchester.h"
#include "../../lib/serial/serial.h"

int main (){
	DDRC = 0xFF;
	PORTC = 0x00;
	
	wdt_disable();


	serial_init_b(38400);
	manchester_init_rx(300);
		
	//char s;
	
	uint8_t data[32];
	
	while (1){
		manchester_read(data, 32);
		serial_write_s(data);
	}   
}
