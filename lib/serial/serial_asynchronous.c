#include "serial.h"
#include <avr/interrupt.h>

#define BUFFER_SIZE 32

struct ring {
	uint8_t buffer[BUFFER_SIZE];
	uint8_t start;
	uint8_t end;
};

static struct ring tx_buffer;
static struct ring rx_buffer;

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
	
	//Enable interrupts
	UCSR0B |= _BV(RXCIE0);// | _BV(UDRIE0) | _BV(TXCIE0);
	
	sei();
}

void serial_init_b(uint16_t baud){
	serial_init(baud, 8, 0, 1);
}

uint8_t serial_read_c(char *c){
	//Next index, assuming all goes well
	uint8_t next_start = (rx_buffer.start + 1) % BUFFER_SIZE;
	
	//Return the next byte if we have remaining items in the buffer
	if (next_start != rx_buffer.end) {
		rx_buffer.start = next_start;
		*c = rx_buffer.buffer[next_start];
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t serial_read_s(char *s, uint8_t len){
	uint8_t count = 0;
	char data = 0;
	
	while (count < len && serial_read_c(&data)){
		s[count++] = data;
	}
	
	return count;
}

void serial_write_s(char *data){
	while (*data){
		serial_write_c(*data++);
	}
}

void serial_write_c(char data){
	//Next index, assuming all goes well
	uint8_t next_end = (tx_buffer.end + 1) % BUFFER_SIZE;

	//Buffer the next byte if there is room left in the buffer.
	if (next_end != tx_buffer.start){
		tx_buffer.buffer[tx_buffer.end] = data;
		tx_buffer.end = next_end;
	}
	
	//Signal that there is data available; the UDRE interrupt will fire.
	UCSR0B |= _BV(UDRIE0);
}

ISR(USART_RX_vect){
	//Next index, assuming all goes well
	uint8_t next_end = (rx_buffer.end + 1) % BUFFER_SIZE;

	//Buffer the next byte if there is room left in the buffer.
	if (next_end != rx_buffer.start){
		rx_buffer.buffer[rx_buffer.end] = UDR0;
		rx_buffer.end = next_end;
	}
} 

ISR(USART_UDRE_vect){
	//Next index, assuming all goes well
	uint8_t next_start = (tx_buffer.start + 1) % BUFFER_SIZE;
	
	//Transmit the next byte if we have remaining items in the buffer
	if (next_start != tx_buffer.end) {
		UDR0 = tx_buffer.buffer[next_start];
		tx_buffer.start = next_start;
	}
	else {
		UCSR0B &= ~_BV(UDRIE0);
	}
}
