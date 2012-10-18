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

#ifndef twi_h
#define twi_h

	#include <inttypes.h>

	//#define ATMEGA8

	#ifndef TWI_FREQ
	#define TWI_FREQ 400000L
	#endif

	#ifndef TWI_BUFFER_LENGTH
	#define TWI_BUFFER_LENGTH 32
	#endif
	
	//Constants used for read / write methods: send / no send stop bit,
	// block on function / no block on function
	#define TWI_STOP			1
	#define TWI_NO_STOP		0
	#define TWI_BLOCK			1
	#define TWO_NO_BLOCK		0

	#define TWI_READY 0
	#define TWI_MRX	 1
	#define TWI_MTX	 2
	#define TWI_SRX	 3
	#define TWI_STX	 4
	
	void twi_init();
	void twi_set_slave_address(uint8_t);
	uint8_t twi_read_from(uint8_t, uint8_t*, uint16_t, uint8_t);
	uint8_t twi_write_to(uint8_t, uint8_t*, uint16_t, uint8_t, uint8_t);
	uint8_t twi_transmit(const uint8_t*, uint16_t);
	void twi_attach_slave_rx_event( void (*)(uint8_t*, uint16_t) );
	void twi_attach_slave_tx_event( void (*)(void) );
	void twi_reply(uint8_t);
	void twi_stop(void);
	void twi_release_bus(void);

#endif

