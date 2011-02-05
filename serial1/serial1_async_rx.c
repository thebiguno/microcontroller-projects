/*
 * An asynchronous implementation of the rx portion of the serial library.  This will let you 
 * avoid the need to poll for incoming data.
 */

#include "serial1.h"
#include "../ring/ring.h"
#include <avr/interrupt.h>

static volatile ring_t rx_buffer;

void _serial1_init_rx(){
	ring_init(&rx_buffer, SERIAL_BUFFER_SIZE);

	//Enable RX interrupts
	UCSR1B |= _BV(RXCIE1);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif	
}

uint8_t serial1_available() {
	UCSR1B &= ~_BV(RXCIE1); //Temporarily disable RX interrupts so we don't get interrupted
	uint8_t r = !_buffer_empty(&rx_buffer);
	UCSR1B |= _BV(RXCIE1); //Re-enable interrupts
	return r;
}

uint8_t serial1_read_c(char *c){
	UCSR1B &= ~_BV(RXCIE1); //Temporarily disable RX interrupts so we don't get interrupted
	if (!_buffer_empty(&rx_buffer)){
		*c = _buffer_get(&rx_buffer);
		UCSR1B |= _BV(RXCIE1); //Re-enable interrupts
		return 1;
	}
	UCSR1B |= _BV(RXCIE1); //Re-enable interrupts
	return 0;
}

uint8_t serial1_read_s(char *s, uint8_t len){
	uint8_t count = 0;
	char data = 0;
	
	while (count < (len - 1) && serial1_read_c(&data)){
		s[count++] = data;
	}
	
	s[count++] = 0x00;
	
	return count;
}

ISR(USART1_RX_vect){
	char data = UDR1;
	if (!_buffer_full(&rx_buffer)){
		_buffer_put(&rx_buffer, data);
	}
}
