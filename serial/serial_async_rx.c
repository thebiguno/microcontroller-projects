/*
 * An asynchronous implementation of the rx portion of the serial library.  This will let you 
 * avoid the need to poll for incoming data.
 */

#include "serial.h"
#include <avr/interrupt.h>

static volatile struct ring rx_buffer;

static inline uint8_t _buffer_empty(volatile struct ring *buffer){
	return (buffer->head == buffer->tail);
}

static inline uint8_t _buffer_full(volatile struct ring *buffer){
	return ((buffer->head + 1) % SERIAL_BUFFER_SIZE == buffer->tail);
}

static inline char _buffer_get(volatile struct ring *buffer){
	char c = buffer->buffer[buffer->tail];
	if (++buffer->tail >= SERIAL_BUFFER_SIZE) buffer->tail = 0;
	return c;
}

static inline void _buffer_put(volatile struct ring *buffer, char data){
	buffer->buffer[buffer->head] = data;
	if (++buffer->head >= SERIAL_BUFFER_SIZE) buffer->head = 0;
}

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
	uint8_t r = !_buffer_empty(&rx_buffer);
	UCSR0B |= _BV(RXCIE0); //Re-enable interrupts
	return r;
}

uint8_t serial_read_c(char *c){
	UCSR0B &= ~_BV(RXCIE0); //Temporarily disable RX interrupts so we don't get interrupted
	if (!_buffer_empty(&rx_buffer)){
		*c = _buffer_get(&rx_buffer);
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
	if (!_buffer_full(&rx_buffer)){
		_buffer_put(&rx_buffer, data);
	}
}
