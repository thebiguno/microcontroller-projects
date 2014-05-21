#include "serial.h"

void _serial_init_tx(){}

void serial_write_s(char *data){
	while (*data){
		serial_write_c(*data++);
	}
}

void serial_write_c(char data){
	//Nop loop to wait until last transmission has completed
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = data;
}

void serial_write_b(uint8_t data){
	serial_write_c((char) data);
}

void serial_write_a(uint8_t *data, uint8_t len){
	for (uint8_t i = 0; i < len; i++){
		serial_write_b(data[i]);
	}
}
