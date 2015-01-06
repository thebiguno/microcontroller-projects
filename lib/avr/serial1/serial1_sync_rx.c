#include "serial1.h"

void _serial1_init_rx(){}

uint8_t serial1_available() {
    return UCSR1A & _BV(RXC1);
}

uint8_t serial1_read_c(char *c){
	while (!(UCSR1A & _BV(RXC1)));
	*c = UDR1;
	return 1;
}

uint8_t serial1_read_s(char *s, uint8_t len){
	uint8_t count = 0;
	char data = 0;
	
	while (count < len && serial1_read_c(&data)){
		s[count++] = data;
	}
	
	return count;
}
