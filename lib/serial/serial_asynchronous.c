#include "serial.h"
#include <avr/interrupt.h>

#define TX_BUFFER_SIZE 64
#define RX_BUFFER_SIZE 64

//Circular transmit buffer.  User code writes to (and increments) 
// tx_end, and interrupts transmit bits located at
// tx_start one byte at a time until start == end.
static char tx_buffer[TX_BUFFER_SIZE];
static uint8_t tx_start = 0;
static uint8_t tx_end = 0;

//Circular receive buffer.  Interrupts receive bytes, and copy to index
// rx_end.  User code can then read butes from rx_start
// until start == end.
static char rx_buffer[RX_BUFFER_SIZE];
static uint8_t rx_start = 0;
static uint8_t rx_end = 0;

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
	UCSR0B |= _BV(TXCIE0) | _BV(RXCIE0); // | _BV(UDRIE0);
	
	sei();
}

void serial_init_b(uint16_t baud){
	serial_init(baud, 8, 0, 1);
}

/*
uint8_t _serial_check_rx_complete(){
	//The RXC0 bit in UCSR0A register is set to 1 when data is
	// available (has been received over the Serial link) and
	// it has not yet been read.  When this is set, the Receive
	// Complete Interrupt is generated (if RXCIE0 bit in UCSR0B
	// is set - this enables the receive interrupts.)
	return(UCSR0A & _BV(RXC0));
}

uint8_t _serial_check_tx_ready(){
	//UCRE0 is 'USART Data Register Empty'.  This is set to 1 when 
	// the UDR0 register is empty, and thus another byte is ready
	// to be inserted into it.  This data is kept in UCSR0A, at bit
	// position UDRE0.
	return(UCSR0A & _BV(UDRE0));
}
*/

char serial_read_c(){
	//Loop to first index if we go past buffer size
	if (rx_start >= RX_BUFFER_SIZE) rx_start = 0;
	
	//If we have read all available data, we return 0.
	if (rx_start == rx_end) return 0;
	
	//If there is data left to read, we return it and increment the last_read pointer.
	return rx_buffer[rx_start++];
}

void serial_read_s(char *data, uint8_t length){
	int count = 0;
	
	for (int i = 0; i < length; i++){
		data[i] = serial_read_c();
		if (data[i] != 0) count++;
	}
}

//The actual worker function for transmitting data.  Called from the TX vector to check 
// if there is any more data to send, and also called from the serial_write_* 
// functions to start the data transmitting.
void _serial_write_data(){
	//Loop to first index if we go past buffer size
	if (tx_start >= TX_BUFFER_SIZE) tx_start = 0;
		
	//Transmit the next byte if we have remaining items in the buffer, and 
	// if the last transmission has finished.
	if (tx_start != tx_end && UCSR0A & _BV(UDRE0)) {
		UDR0 = tx_buffer[tx_start++];
	}
}

void serial_write_s(char *data){
	while (*data){
		//Loop if needed
		if (tx_end >= TX_BUFFER_SIZE) tx_end = 0;
	
		//Store data to the buffer for later transmission
		tx_buffer[tx_end++] = *data++;
	}
	
	_serial_write_data();
}

void serial_write_c(char data){
	//Loop if needed
	if (tx_end >= TX_BUFFER_SIZE) tx_end = 0;
	
	//Store data to the buffer for later transmission
	tx_buffer[tx_end++] = data;
	
	_serial_write_data();
}

ISR(USART_TX_vect){
	_serial_write_data();
}

ISR(USART_RX_vect){
	//Loop to first index if we go past buffer size
	if (rx_end >= RX_BUFFER_SIZE) rx_end = 0;
	
	//Read the next byte into the buffer
	rx_buffer[rx_end++] = UDR0;
} 
