/*
 * An asynchronous implementation of the rx portion of the serial library.  This will let you 
 * avoid the need to poll for incoming data.
 */

#include "serial1.h"
#include "../Ring/Ring.h"
#include <avr/interrupt.h>

static Ring rx_buffer(SERIAL_BUFFER_SIZE);

void _serial1_init_rx(){
	//Enable RX interrupts
	UCSR1B |= _BV(RXCIE1);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif	
}

uint8_t serial1_available() {
	UCSR1B &= ~_BV(RXCIE1); //Temporarily disable RX interrupts so we don't get interrupted
	uint8_t r = !rx_buffer.isEmpty();
	UCSR1B |= _BV(RXCIE1); //Re-enable interrupts
	return r;
}

uint8_t serial1_read_c(char *c){
	UCSR1B &= ~_BV(RXCIE1); //Temporarily disable RX interrupts so we don't get interrupted
	if (!rx_buffer.isEmpty()){
		*c = rx_buffer.get();
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
	if (!rx_buffer.isFull()){
		rx_buffer.put(data);
	}
}
