#include "serial.h"

/*
 * Initializes the USART with the given parameters.  Valid arguments include:
 *  baud: Any valid baud rate based on hardware support
 *  dataBits: Between 5 and 9 inclusive.  8 recommended.
 *  parity: 0 (no parity bit), 2 (even parity), 3 (odd parity).  0 or 2 recommended.
 *  stopBits: 1 or 2.
 */
void serial_init(unsigned int baud, unsigned char data_bits, unsigned char parity, unsigned char stop_bits){  
	//Set baud rate
	unsigned int calculated_baud = (F_CPU / 16 / baud) - 1;
	UBRR0H = (unsigned char) (calculated_baud >> 8);
	UBRR0L = (unsigned char) calculated_baud;

	
	//Calculate frame format
	unsigned char frame_format = 0x0;
	
	//Data bits
	if (data_bits == 9){
		//9 bits use an extra bit in register UCSR0B
		UCSR0B |= (1 << UCSZ02);
		frame_format |= (1 << UCSZ01) | (1 << UCSZ00);
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
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
	
	//Enable interrupts if desired
//	if (enableInterrupts)
//		UCSR0B |= (1 << TXCIE0) | (1 << RXIE0) | (1 << UDRIE0);
}

/*
 * Checks if there is any serial data available to read.  Returns 
 * a non-zero if there is anything available, 0 otherwise.
 */
char serial_check_rx_complete(){
	//The RXC0 bit in UCSR0A register is set to 1 when data is
	// available (has been received over the Serial link) and
	// it has not yet been read.  When this is set, the Receive
	// Complete Interrupt is generated (if RXCIE0 bit in UCSR0B
	// is set - this enables the receive interrupts.)
	return(UCSR0A & (1 << RXC0));
}

/*
 * Checks if the serial data register UDR0 is empty, and ready
 * to be re-populated with another byte.  Returns 0 if this is
 * not ready, non-zero otherwise.
 */
char serial_check_tx_ready(){
	//UCRE0 is 'USART Data Register Empty'.  This is set to 1 when 
	// the UDR0 register is empty, and thus another byte is ready
	// to be inserted into it.  This data is kept in UCSR0A, at bit
	// position UDRE0.
	return(UCSR0A & (1 << UDRE0));
}

/*
 * Reads a single character from the serial port if available.  Blocks until
 * data is read.
 */
char serial_read(){
	// Nop loop while data is not available for reading
	while (!serial_check_rx_complete());
	return UDR0;
}

/*
 * Writes a string to the serial port.  Blocks until the string
 * is written.
 */
void serial_write_s(char* data){
	int i;
	for (i = 0; i < strlen(data); i++){
		serial_write_c(data[i]);
	}
}

/*
 * Writes a single byte to the serial port.  Blocks until the 
 * data register UDR0 is available, and then popualtes it.
 */
void serial_write_c(char data){
	while (serial_check_tx_ready() == 0);
	UDR0 = data;
}
