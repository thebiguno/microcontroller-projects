#include "serial1.h"

void _serial1_init_tx(){}

void serial1_write_s(char *data){
	while (*data){
		serial1_write_c(*data++);
	}
}

void serial1_write_c(char data){
	//Nop loop to wait until last transmission has completed
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = data;
}
