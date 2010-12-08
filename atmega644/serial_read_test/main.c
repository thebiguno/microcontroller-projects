#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SERIAL_BUFFER_SIZE 256

struct ring {
	//Just like a snake eating something and pooping it out...
	volatile uint16_t head;  //You put data into head...
	volatile uint16_t tail;  //...and take it off of tail.
	//If they are equal, then there is nothing in the buffer.  If 
	// (head + 1) % length == tail then the buffer is full.  The current
	// position of head points to the location where the next byte will
	// be written (and head will then be incremented after the byte is written); 
	// the position of tail points to the location of the last byte which was read,
	// and must be incremented before the next read.
	uint8_t buffer[SERIAL_BUFFER_SIZE];
};

static struct ring rx_buffer;
static struct ring tx_buffer;

inline uint8_t _buffer_empty(struct ring *buffer){
	return (buffer->head == buffer->tail);
}

inline uint8_t _buffer_full(struct ring *buffer){
	return ((buffer->head + 1) % SERIAL_BUFFER_SIZE == buffer->tail);
}

inline char _buffer_get(struct ring *buffer){
	if (buffer->head != buffer->tail){
		char data = buffer->buffer[buffer->tail];
		buffer->tail++;
		if (buffer->tail >= SERIAL_BUFFER_SIZE) buffer->tail = 0;
		return data;
	}
	return 'W';
}

inline void _buffer_put(struct ring *buffer, char data){
	if (!_buffer_full(buffer)){
		buffer->buffer[buffer->head] = data;
		buffer->head++;
		if (buffer->head >= SERIAL_BUFFER_SIZE) buffer->head = 0;
	}
}

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
	
	//Enable RX interrupts; UDRE will be enabled on serial_write_c().
	UCSR0B |= _BV(RXCIE0);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void serial_init_b(uint32_t baud){
	serial_init(baud, 8, 0, 1);
}

void serial_write_c(char data){
	_buffer_put(&tx_buffer, data);
	
	//Signal that there is data available; the UDRE interrupt will fire.
	UCSR0B |= _BV(UDRIE0);
}


int main (){
	serial_init_b(57600);
	
	DDRB |= _BV(PINB0) | _BV(PINB1) | _BV(PINB4);

	while (1){
		PORTB ^= _BV(PINB4);
	
		if (!_buffer_empty(&rx_buffer)){
			UCSR0B |= _BV(UDRIE0);
			PORTB |= _BV(PINB0);
		}
		else {
			PORTB &= ~_BV(PINB0);
		}
	}   
}

ISR(USART0_RX_vect){
	char data = UDR0;
	_buffer_put(&rx_buffer, data);
}

ISR(USART0_UDRE_vect){
	if (!_buffer_empty(&rx_buffer)){
		char data = _buffer_get(&rx_buffer);
		UDR0 = data;
	}
	else {
		//Once the ring buffer is empty (i.e. head == tail), we disable
		// USART Data Register Empty interrupts until someone write to
		// the tx buffer again.
		UCSR0B &= ~_BV(UDRIE0);
	}

}
