/*
 * An asynchronous implementation of the serial library.  This will let you submit a bunch
 * of data to a buffer, and let the AVR send it using interrupts.  This results in shorter
 * send / receive times (as the write / read methods do not block), but there are downsides.
 * It a) causes code to be run in interrupts (which may interere with timing-critical code),
 * b) adds substantially to the total SRAM usede by a program (defaults to two 64 byte 
 * buffers for transmitting and receiving, although this buffer size can be configured at
 * compile time), c) introduces the possibility that data will be partially unsent, if 
 * the program tries to write too much at a time and overflows the buffer, and d) for single
 * byte messages which are not sent continuously, there is actually a performance decrease 
 * as there is an extra layer of abstraction (the buffer) in between the hardware and the 
 * sending program.
 * 
 * With the downsides, though, come a definite advantage -- for longer strings (experiments
 * show this seems to be effective for more than 2 bytes), the speed is much faster.  When
 * sending 3 bytes, it takes 1215µs (1.2ms) to return from the synchronous implementation;
 * while taking only 280µs (0.3ms) to return from the asynchronous one.  For longer 
 * strings the difference is even more pronounced: if you send a 26 character string, it 
 * will take the synchronous implementation about 25131µs (25ms) to return from the 
 * write_s function; the asynchronous implementation takes about 669µs (less than 1ms) 
 * to return.
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

void _serial_write_to_buffer(struct ring *buffer, char data){
	//Next index, assuming all goes well
	uint8_t next_head = (buffer->head + 1) % SERIAL_BUFFER_SIZE;

	//Buffer the next byte if there is room left in the buffer.
	if (next_head != buffer->tail){
		buffer->buffer[buffer->head] = data;
		buffer->head = next_head;
	}
}

char _serial_read_from_buffer(struct ring *buffer, char *c){
	//Return the next byte if we have remaining items in the buffer
	if (buffer->head == buffer->tail) {
		return 0;
	}
	else {
		*c = buffer->buffer[buffer->tail];
		buffer->tail = (buffer->tail + 1) % SERIAL_BUFFER_SIZE;	
		return 1;
	}
}

static struct ring tx_buffer;
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
	UCSR0B |= _BV(RXCIE0);// | _BV(UDRIE0) | _BV(TXCIE0);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void serial_init_b(uint32_t baud){
	serial_init(baud, 8, 0, 1);
}

uint8_t serial_read_c(char *c){
	if (_serial_read_from_buffer(&rx_buffer, c)){
		return 1;
	}
	return 0;
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
	_serial_write_to_buffer(&tx_buffer, data);
	
	//Signal that there is data available; the UDRE interrupt will fire.
	UCSR0B |= _BV(UDRIE0);
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
	unsigned char data = UDR0;
	_serial_write_to_buffer(&rx_buffer, data);
} 


#if defined(__AVR_ATtiny2313__)    || \
	defined(__AVR_ATmega48P__)     || \
	defined(__AVR_ATmega88P__)     || \
	defined(__AVR_ATmega168P__)    || \
	defined(__AVR_ATmega328P__)    || \
	defined(__AVR_ATmega48__)      || \
	defined(__AVR_ATmega88__)      || \
    defined(__AVR_ATmega168__)
ISR(USART_UDRE_vect){
#elif defined(__AVR_ATmega324P__)  || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)
ISR(USART0_UDRE_vect){
#else 
#error You must define USART vectors for your chip!  Please verify that MMCU is set correctly, and that there is a matching vector definition in serial_asynchronous.c
void error2() {
#endif
	char c;
	if (_serial_read_from_buffer(&tx_buffer, &c)){
		UDR0 = c;
	}
	else {
		//Once the ring buffer is empty (i.e. head == tail), we disable
		// USART Data Register Empty interrupts until someone write to
		// the tx buffer again.
		UCSR0B &= ~_BV(UDRIE0);
	}
}

