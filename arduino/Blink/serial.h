#include <avr/io.h>
#include <string.h>

/*
 * Initializes the USART with the given parameters.  Valid arguments include:
 *  baud: Any valid baud rate based on hardware support
 *  dataBits: Between 5 and 9 inclusive.  8 recommended.
 *  parity: 0 (no parity bit), 2 (even parity), 3 (odd parity).  0 or 2 recommended.
 *  stopBits: 1 or 2.
 *  interruptsEnabled: if true, enable interrupts for Rx Complete, Tx Complete, and 
 *       USART Data Register Empty
 */
void serial_init(unsigned int baud, unsigned char dataBits, unsigned char parity, unsigned char stopBits);

/*
 * Checks if there is any serial data available to read.  Returns 
 * a non-zero if there is anything available, 0 otherwise.
 */
char serial_check_rx_complete();

/*
 * Checks if the serial data register UDR0 is empty, and ready
 * to be re-populated with another byte.  Returns 0 if this is
 * not ready, non-zero otherwise.
 */
char serial_check_tx_ready();

/*
 * Reads a single character from the serial port.  Blocks until
 * data is read.
 */
char serial_read();

/*
 * Writes a string to the serial port.  Blocks until the string
 * is written.
 */
void serial_write_s(char* data);

/*
 * Writes a single byte to the serial port.  Blocks until the 
 * data register UDR0 is available, and then popualtes it.
 */
void serial_write_c(char data);
