#include <avr/io.h>
#include "lib/serial/serial.h"
#include "lib/Distance/Distance.h"

using namespace digitalcave;

int main (void){
	//Do setup here
	PORTC = 0x00;
	
	Distance d(&PORTC, PORTC5, &PINC, PINC4);
	
	DDRC |= _BV(PORTC5);	//A5 output mode
	
	//1ms is this many clock ticks
#define TEN_MICROSECONDS (F_CPU / 100000)
	
	serial_init_b(9600);
	
	//Main program loop
	while (1){
		uint16_t distance = d.read();
		
		serial_write_b((distance >> 8) & 0xFF);
		serial_write_b(distance & 0xFF);
		
		_delay_ms(1000);
	}
}

