#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"

void serial_buffered_init(unsigned int baud, unsigned char data_bits, unsigned char parity, unsigned char stop_bits);

/*
 * Returns the first
 */
char serial_buffered_read_c();

/*
 * Return a string which contains all buffered characters, or a zero
 * character string if there are no unread characters in the buffer.
 */
char* serial_buffered_read_s();
