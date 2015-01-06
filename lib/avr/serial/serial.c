/*
 * A modular implementation of serial library.  You only need to include those
 * parts which you need.
 *
 * This module defines the init methods; add rx / tx modules as you need.
 */

#include "serial.h"

//Defined in rx / tx modules
void _serial_init_rx();
void _serial_init_tx();

void serial_init(uint32_t baud, uint8_t data_bits, uint8_t parity, uint8_t stop_bits){
	//Set baud rate
	uint16_t calculated_baud = (F_CPU / 16 / baud) - 1;
	UBRR0H = calculated_baud >> 8;
	UBRR0L = calculated_baud & 0xFF;

	//Make sure 2x and multi processor comm modes are off
	UCSR0A &= ~(_BV(U2X0) | _BV(MPCM0));
	
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
	
	//Enable Rx / Tx if they are not disabled, and call init methods of rx/tx modules
#ifndef SERIAL_DISABLE_RX
	UCSR0B |= _BV(RXEN0);
	_serial_init_rx();
#endif
#ifndef SERIAL_DISABLE_TX
	UCSR0B |= _BV(TXEN0);
	_serial_init_tx();
#endif
}

void serial_init_b(uint32_t baud){
	serial_init(baud, 8, 0, 1);
}

