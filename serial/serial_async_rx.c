/*
 * An asynchronous implementation of the rx portion of the serial library.  This will let you 
 * avoid the need to poll for incoming data.
 */

#include "serial.h"
#include "../Ring/Ring.h"
#include <avr/interrupt.h>

static Ring rx_buffer(SERIAL_BUFFER_SIZE);

void _serial_init_rx(){
	//Enable RX interrupts
	UCSR0B |= _BV(RXCIE0);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif	
}

uint8_t serial_available() {
	UCSR0B &= ~_BV(RXCIE0); //Temporarily disable RX interrupts so we don't get interrupted
	uint8_t r = !rx_buffer.isEmpty();
	UCSR0B |= _BV(RXCIE0); //Re-enable interrupts
	return r;
}

uint8_t serial_read_c(char *c){
	UCSR0B &= ~_BV(RXCIE0); //Temporarily disable RX interrupts so we don't get interrupted
	if (!rx_buffer.isEmpty()){
		*c = rx_buffer.get();
		UCSR0B |= _BV(RXCIE0); //Re-enable interrupts
		return 1;
	}
	UCSR0B |= _BV(RXCIE0); //Re-enable interrupts
	return 0;
}

uint8_t serial_read_s(char *s, uint8_t len){
	uint8_t count = 0;
	char data = 0;
	
	while (count < (len - 1) && serial_read_c(&data)){
		s[count++] = data;
	}
	
	s[count++] = 0x00;
	
	return count;
}


//Note: These defines are only a small subset of those which are available (and are
// pretty much only the chips which I personally use).  You can add more chips to 
// these defines, and as long as you match the correct chips with the correct vector
// names, it should just work.
#if defined(__AVR_ATtiny2313__)    || \
	defined(__AVR_ATmega48P__)     || \
	defined(__AVR_ATmega88P__)     || \
	defined(__AVR_ATmega168P__)    || \
	defined(__AVR_ATmega328P__)    || \
	defined(__AVR_ATmega48__)      || \
	defined(__AVR_ATmega88__)      || \
    defined(__AVR_ATmega168__)
ISR(USART_RX_vect){
#elif defined(__AVR_ATmega324P__)  || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)
ISR(USART0_RX_vect){
#else 
#error You must define USART vectors for your chip!  Please verify that MMCU is set correctly, and that there is a matching vector definition in serial_asynchronous.c
void error1() {
#endif
	char data = UDR0;
	if (!rx_buffer.isFull()){
		rx_buffer.put(data);
	}
}
