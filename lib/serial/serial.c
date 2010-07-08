#include "serial.h"

void serial_init(uint16_t baud, uint8_t data_bits, uint8_t parity, uint8_t stop_bits){  
	//Set baud rate
	unsigned int calculated_baud = (F_CPU / 16 / baud) - 1;
	UBRR0H = calculated_baud >> 8;
	UBRR0L = calculated_baud & 0xFF;

	
	//Calculate frame format
	uint8_t frame_format = 0x0;
	
	//Data bits
	if (data_bits == 9){
		//9 bits use an extra bit in register UCSR0B
		UCSR0B |= _BV(UCSZ02);
		frame_format |= _BV(UCSZ01) | _BV(UCSZ00);
	}
	else {
		frame_format |= ((data_bits - 5) << UCSZ00);
	}
	
	//Parity
	frame_format |= (parity << UPM00);
	
	//Stop Bits
	frame_format |= ((stop_bits - 1) << USBS0);
	
	//Set frame format
	UCSR0C = frame_format;
	
	
	//Enable Rx / Tx
	UCSR0B |= _BV(TXEN0) | _BV(RXEN0);
	
	//Enable interrupts if desired
//	if (enableInterrupts)
//		UCSR0B |= _BV(TXCIE0) | _BV(RXIE0) | _BV(UDRIE0);
}

void serial_init_b(uint16_t baud){
	serial_init(baud, 8, 0, 1);
}

uint8_t serial_check_rx_complete(){
	//The RXC0 bit in UCSR0A register is set to 1 when data is
	// available (has been received over the Serial link) and
	// it has not yet been read.  When this is set, the Receive
	// Complete Interrupt is generated (if RXCIE0 bit in UCSR0B
	// is set - this enables the receive interrupts.)
	return(UCSR0A & _BV(RXC0));
}

uint8_t serial_check_tx_ready(){
	//UCRE0 is 'USART Data Register Empty'.  This is set to 1 when 
	// the UDR0 register is empty, and thus another byte is ready
	// to be inserted into it.  This data is kept in UCSR0A, at bit
	// position UDRE0.
	return(UCSR0A & _BV(UDRE0));
}

char serial_read(){
	// Nop loop while data is not available for reading
	while (!serial_check_rx_complete());
	return UDR0;
}

void serial_write_s(char *data){
	while (*data){
		serial_write_c(*data++);
	}
}

void serial_write_c(char data){
	while (serial_check_tx_ready() == 0);
	UDR0 = data;
}
