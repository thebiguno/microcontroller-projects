/*
	twi.h - TWI/I2C library for Wiring & Arduino
	Copyright (c) 2006 Nicholas Zambetti.	All right reserved.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA	02110-1301	USA
*/

//You can set the following defines (either in code or in the Makefile) to modify the behaviour of the system:
//TWI_FREQ						Set the TWI frequency, in Hz.  Defaults to 400000L (400KHz)
//TWI_BUFFER_LENGTH				Maximum buffer length, in bytes; even needed if you provide custom buffers.  Defaults to 32
//TWI_DISABLE_MASTER			Completely disable master mode
//TWI_DISABLE_SLAVE				Completely disable slave mode
//TWI_DISABLE_SLAVE_RX			Disable RX in slave mode
//TWI_DISABLE_SLAVE_TX			Disable TX in slave mode
//TWI_CUSTOM_BUFFERS			Set a custom buffer for master, slave rx, and slave tx.  See twi_set_*_buffer
//TWI_SLAVE_RX_READER			Set a reader function to handle each byte as it comes in.  See twi_attach_slave_rx_reader
//TWI_MASTER_RX_READER			Set a reader function to handle each byte as it comes in.  See twi_attach_master_rx_reader
//TWI_SLAVE_TX_WRITER			Set a writer function to supply each byte as it is sent.  See twi_attach_slave_tx_writer
//TWI_MASTER_TX_WRITER			Set a writer function to supply each byte as it is sent.  See twi_attach_slave_tx_writer


#ifndef TWI_H
#define TWI_H

#include <inttypes.h>

#ifndef TWI_FREQ
#define TWI_FREQ 400000L
#endif

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 32
#endif

//Constants used for read / write methods: send / no send stop bit,
// block on function / no block on function
//Make sure these map to the same values as the I2C versions in common/I2C.h
#define TWI_STOP		1
#define TWI_NO_STOP		0
#define TWI_BLOCK		1
#define TWI_NO_BLOCK	0

#define TWI_READY 0
#define TWI_MRX	 1
#define TWI_MTX	 2
#define TWI_SRX	 3
#define TWI_STX	 4

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Override the default buffers with your own buffers.  This gives you more control over how the
 * buffers are constructed.
 */
void twi_set_master_buffer(uint8_t* buffer);
void twi_set_rx_buffer(uint8_t* buffer);
void twi_set_tx_buffer(uint8_t* buffer);

/*
 * For even more control, you can attach readers / writers to the master / slave; this lets
 * you programatically construct the data to send / handle the data received, without just
 * relying on a sequential buffer.
 */
//Attach a slave rx reader function.  This takes a uint8_t (incoming data), and uint16_t (incoming data's index).
void twi_attach_slave_rx_reader( void (*function)(uint8_t, uint16_t) );

//Attach a slave tx writer function.  This takes a uint16_t (outgoing data's index), and returns a uint8_t (the
// byte to be transmitted).
void twi_attach_slave_tx_writer( uint8_t (*function)(uint16_t) );

//Attach a master rx reader.  This takes a uint8_t (incoming data), and uint16_t (incoming data's index).
void twi_attach_master_rx_reader( void (*function)(uint8_t, uint16_t) );

//Attach a master tx writer.  This takes a uint16_t (outgoing data's index), and returns a uint8_t (the
// byte to be transmitted.
void twi_attach_master_tx_writer( uint8_t (*function)(uint16_t) );

/*
 * The core TWI functions: send and receive data, from either master or slave.
 */
//Initialize the TWI hardware
void twi_init();

//Master mode, read from the given address, to the given data buffer, for the given number of
// bytes, and either TWI_STOP or TWI_NO_STOP when completed.
uint8_t twi_read_from(uint8_t address, uint8_t* data, uint16_t length, uint8_t send_stop);

//Master mode, write to the given slave address, from the given data buffer, for the given number of
// bytes.  If block is TWI_BLOCK (not TWI_NO_BLOCK) then this function blocks until the write is
// completed.  If send_stop is TWI_STOP then we send the stop byte when completed.
uint8_t twi_write_to(uint8_t address, uint8_t* data, uint16_t length, uint8_t block, uint8_t send_stop);

//Slave mode, start listening on the specified address.
void twi_set_slave_address(uint8_t);

//Transmit is called from the slave tx event callback, and will start the transmission back to the master.
uint8_t twi_transmit(const uint8_t*, uint16_t);

//Attach the supplied callback function, which is called when the slave rx is completed.  The
// callback's arguments are the daat buffer which was just read, and the length of that buffer.
// This function is most likely redundant if TWI_SLAVE_RX_READER is defined (that callback will
// be called once for each byte in the incoming message, whereas this one will not be called
// until the entire message is buffered, but there are probably not many scenarios when both
// approaches are valid at the same time.)
void twi_attach_slave_rx_callback( void (*)(uint8_t*, uint16_t) );

//Attach the supplied callback function, which is called when the slave receives a tx request.
// There are no arguments to the callback function; the callback must assemble the data buffer
// and submit it via the twi_transmit() function.  This function is most likely redundant if
// TWI_SLAVE_TX_WRITER is defined (that callback will write one byte at a time, whereas this
// one will assemble the entire message, but the end result is essentially the same.)
void twi_attach_slave_tx_callback( void (*)(void) );

#ifdef __cplusplus
}
#endif

#endif
