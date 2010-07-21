#include "serial.h"

void serial_init(uint16_t baud, uint8_t data_bits, uint8_t parity, uint8_t stop_bits){  
	//Set baud rate
	unsigned int calculated_baud = (F_CPU / 16 / baud) - 1;
	UBRR0H = calculated_baud >> 8;
	UBRR0L = calculated_baud & 0xFF;

	
	//Calculate frame format
	//Init to 0; we populate this later
	UCSR0C = 0x0;
	
	//Data bits
	if (data_bits == 9){
		//9 bits use an extra bit in register UCSR0B
		UCSR0B |= _BV(UCSZ02);
		UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
	}
	else {
		UCSR0C |= ((data_bits - 5) << UCSZ00);
	}
	
	//Parity, Stop bits
	UCSR0C |= (parity << UPM00) | ((stop_bits - 1) << USBS0);
	
	
	//Enable Rx / Tx
	UCSR0B |= _BV(TXEN0) | _BV(RXEN0);
}

void serial_init_b(uint16_t baud){
	serial_init(baud, 8, 0, 1);
}

/*
 * Reads one character from the serial port.  Blocks until the character is read.
 */
char serial_read_c(){
	// Nop loop while data is not available for reading
	while (!(UCSR0A & _BV(RXC0)));
	return UDR0;
}

/*
 * Reads exactly length characters from the serial port.  Blocks until all characters
 * are read.
 */
void serial_read_s(char *data, uint8_t length){
	for (int i = 0; i < length; i++){
		data[i] = serial_read_c();
	}
}

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
