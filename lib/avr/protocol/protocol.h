#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <avr/io.h>

#define PROTOCOL_ERROR_UNEXPECTED_START_OF_FRAME	1
#define PROTOCOL_ERROR_INVALID_CHECKSUM				2


/*
 * This library allows for bi-directional, multi byte commands to be sent between two devices.  It is designed to be
 * link agnostic; thus, there are a few functions which you need to implement to link this library into your communications.
 * Be sure to read all the function comments in this file to see what functions must be implemented, and which ones 
 * must be called from the main code.
 */

/*
 * This function MUST be implemented by the calling code.  This function will be called from protocol_receive_byte when
 * the final byte of a given message has arrived and the checksum is validated.
 * 
 * A possible implementation would be to read the incoming message and put it in a mailbox for later use; another possible 
 * implementation would be to immediately consume the message and act on it.  This function should be as quick as possible
 * to ensure that a new incoming message does not clobber the existing message before it has been consumed.  If you have 
 * flow control, it MAY be good to disable other incoming messages until this message has been handled.
 */
void protocol_dispatch_message(uint8_t cmd, uint8_t *bytes, uint8_t length);

/*
 * This function MUST be implemented by the calling code.  This function's implementation would pass the byte to whatever
 * link layer is in use.
 * 
 * For a default implementation using the serial library, you can use file protocol_serial.c (instead of using serial_*_rx.c)
 */
void protocol_write(uint8_t byte);

/*
 * Receive a single byte from the link layer.  This function MUST be called from the main code whenever a byte is received
 * from the link layer.
 * 
 * For a default implemenation using the serial library, you can use file protocol_serial.c (instead of using serial_*_rx.c)
 */
void protocol_receive_byte(uint8_t b);

/*
 * Send a message, properly escaping it, using the function protocol_write() to send the individual bytes.
 */
void protocol_send_message(uint8_t cmd, uint8_t *bytes, uint8_t length);

/*
 * Gets the latest error status code.  0 means no error, non-zero is error.
 */
uint8_t protocol_get_error();

#endif
