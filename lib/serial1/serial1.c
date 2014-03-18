/*
 * A modular implementation of serial library.  You only need to include those
 * parts which you need.
 *
 * This module defines the init methods; add rx / tx modules as you need.
 */

#include "serial1.h"
#include <avr/interrupt.h>
#include <util/delay.h>

//Defined in rx / tx modules
void _serial1_init_rx();
void _serial1_init_tx();

void serial1_init(uint32_t baud, uint8_t data_bits, uint8_t parity, uint8_t stop_bits){
	//Set baud rate
	uint16_t calculated_baud = (F_CPU / 16 / baud) - 1;
	UBRR1H = calculated_baud >> 8;
	UBRR1L = calculated_baud & 0xFF;

	//Make sure 2x and multi processor comm modes are off
	UCSR1A &= ~(_BV(U2X1) | _BV(MPCM1));
	
	//Calculate frame format
	//Init to 0; we populate this later
	UCSR1C = 0x0;
	
	//Data bits
	if (data_bits == 9){
		//9 bits use an extra bit in register UCSR0B
		UCSR1B |= _BV(UCSZ12);
		UCSR1C |= _BV(UCSZ11) | _BV(UCSZ10);
	}
	else {
		UCSR1C |= ((data_bits - 5) << UCSZ10);
	}
	
	//Parity, Stop bits
	UCSR1C |= (parity << UPM10) | ((stop_bits - 1) << USBS1);
	
	//Enable Rx / Tx if they are not disabled, and call init methods of rx/tx modules
#ifndef SERIAL1_DISABLE_RX
	UCSR1B |= _BV(RXEN1);
	_serial1_init_rx();
#endif
#ifndef SERIAL1_DISABLE_TX
	UCSR1B |= _BV(TXEN1);
	_serial1_init_tx();
#endif
}

void serial1_init_b(uint32_t baud){
	serial1_init(baud, 8, 0, 1);
}

