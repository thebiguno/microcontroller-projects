#ifndef SERIAL_H
#define SERIAL_H

#include <avr/io.h>

/*
 * Initializes the USART with the given parameters.  Valid arguments include:
 *  baud: Any valid baud rate based on hardware support
 *  dataBits: Between 5 and 9 inclusive.  8 recommended.
 *  parity: 0 (no parity bit), 2 (even parity), 3 (odd parity).  0 or 2 recommended.
 *  stopBits: 1 or 2.
 *  interruptsEnabled: if true, enable interrupts for Rx Complete, Tx Complete, and 
 *       USART Data Register Empty
 */
void serial_init(uint16_t baud, uint8_t data, uint8_t parity, uint8_t stopBits);

/*
 * Easy init method which only asks for baud, and gives sane defaults for the rest.
 * Implementations should call serial_init with values baud, 8, 0, 1.
 */
void serial_init_b(uint16_t baud);

/*
 * Reads a single character from the serial port.
 */
char serial_read_c();

/*
 * Reads data into buffer of (at most) the given length.  Returns the number of bytes
 * which were read.
 */
void serial_read_s(char *data, uint8_t length);

/*
 * Writes a string to the serial port.  Blocks until the string
 * is written.
 */
void serial_write_s(char *data);

/*
 * Writes a single byte to the serial port.  Blocks until the 
 * data register UDR0 is available, and then popualtes it.
 */
void serial_write_c(char data);

#endif
