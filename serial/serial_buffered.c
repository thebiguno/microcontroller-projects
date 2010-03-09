#include "serial.c"
#include "serial_buffered.h"

#define BUFFER_SIZE 128

//Circular FIFO buffer, along with head and tail pointers.  When a new character
// is recieved, it is written to the tail; when a character is read, it is
// read from the tail.
char buffer[BUFFER_SIZE];
int head = 0;
int tail = 0;

void serial_buffered_init(unsigned int baud, unsigned char data_bits, unsigned char parity, unsigned char stop_bits){ 
	serial_init(baud, data_bits, parity, stop_bits);
	
	sei();
}

/*
 * Returns the first character in the buffer, or -1 if there is no
 * characters in the buffer.
 */
char serial_buffered_read_c(){
	return -1;
}

/*
 * Return a string which contains all buffered characters, or a zero
 * character string if there are no unread characters in the buffer.
 */
char* serial_buffered_read_s(){
	return "\n";
}

/*
 * This function is called when the USART receives a byte
 */
ISR(USART_RXC_vect){
	
}
