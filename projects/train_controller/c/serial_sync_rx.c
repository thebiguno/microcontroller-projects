#include "serial.h"

void _serial_init_rx(){}

uint8_t serial_available() {
    return UCSR0A & _BV(RXC0);
}

uint8_t serial_read_c(char *c){
	while (!(UCSR0A & _BV(RXC0)));
	*c = UDR0;
	return 1;
}

uint8_t serial_read_s(char *s, uint8_t len){
	uint8_t count = 0;
	char data = 0;
	
	while (count < len && serial_read_c(&data)){
		s[count++] = data;
	}
	
	return count;
}

uint8_t serial_read_b(uint8_t *b){
	return serial_read_c((char*) b);
}

uint8_t serial_read_a(uint8_t *a, uint8_t len){
	return serial_read_s((char*) a, len);
}
