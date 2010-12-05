/*
 * An hybrid asynchronous / sync implementation of the serial library.  This lets you
 * send data synchronously, but does not require you to poll for incoming data.
 */

#include "serial.h"
#include <avr/interrupt.h>

//The buffer size; used for both RX and TX buffers.  Defaults to 64 bytes; you can 
// change it by redefining SERIAL_BUFFER_SIZE in your makefile (in the CDEFS variable,
// beside where F_CPU is defined).
#ifndef SERIAL_BUFFER_SIZE
#define SERIAL_BUFFER_SIZE 64
#endif

struct ring {
	uint8_t buffer[SERIAL_BUFFER_SIZE];
	//Just like a snake eating something and pooping it out...
	uint8_t head;  //You put data into head...
	uint8_t tail;  //...and take it off of tail.
	//If they are equal, then there is nothing in the buffer.  If 
	// (head + 1) % length == tail then the buffer is full.  The current
	// position of head points to the location where the next byte will
	// be written (and head will then be incremented after the byte is written); 
	// the position of tail points to the location of the last byte which was read,
	// and must be incremented before the next read.
};

static struct ring rx_buffer;

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
	
	
	//Enable Rx / Tx
	UCSR0B |= _BV(TXEN0) | _BV(RXEN0);
	
	//Enable interrupts
	UCSR0B |= _BV(RXCIE0);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void serial_init_b(uint32_t baud){
	serial_init(baud, 8, 0, 1);
}

uint8_t serial_read_c(char *c){
	//Return the next byte if we have remaining items in the buffer
	if (rx_buffer.head != rx_buffer.tail) {
		rx_buffer.tail = (rx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
		*c = rx_buffer.buffer[rx_buffer.tail];
		return 1;
	}
	else {
		return 0;
	}
}

uint8_t serial_read_s(char *s, uint8_t len){
	uint8_t count = 0;
	char data = 0;
	
	while (count < len && serial_read_c(&data)){
		s[count++] = data;
	}
	
	return count;
}

void serial_write_s(char *data){
	while (*data){
		serial_write_c(*data++);
	}
}

void serial_write_c(char data){
	//Nop loop to wait until last transmission has completed
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = data;
}

uint8_t serial_available() {
	return (rx_buffer.head == rx_buffer.tail ? 0 : 1);
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
      
	//Next index, assuming all goes well
	uint8_t next_head = (rx_buffer.head + 1) % SERIAL_BUFFER_SIZE;

	//Buffer the next byte if there is room left in the buffer.
	if (next_head != rx_buffer.tail){
		rx_buffer.buffer[rx_buffer.head] = UDR0;
		rx_buffer.head = next_head;
	}
}
