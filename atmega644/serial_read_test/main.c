#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define SERIAL_BUFFER_SIZE 256

struct ring {
	uint8_t buffer[SERIAL_BUFFER_SIZE];
	//Just like a snake eating something and pooping it out...
	uint16_t head;  //You put data into head...
	uint16_t tail;  //...and take it off of tail.
	//If they are equal, then there is nothing in the buffer.  If 
	// (head + 1) % length == tail then the buffer is full.  The current
	// position of head points to the location where the next byte will
	// be written (and head will then be incremented after the byte is written); 
	// the position of tail points to the location of the last byte which was read,
	// and must be incremented before the next read.
};

static struct ring rx_buffer;

int _buffer_count(struct ring *buffer)
{
	// This is the hardest part,
	// calculating the size of data in buffer.
	return ((buffer->head - buffer->tail ) % SERIAL_BUFFER_SIZE);
}

inline char _buffer_get(struct ring *buffer)
{
	char data = buffer->buffer[buffer->tail];
	if (++buffer->tail >= SERIAL_BUFFER_SIZE) buffer->tail = 0;
	return data;
}

inline void _buffer_put(struct ring *buffer, char data){
	if ((buffer->head + 1) % SERIAL_BUFFER_SIZE != buffer->tail){
		buffer->buffer[buffer->head] = data;
		if (++buffer->head >= SERIAL_BUFFER_SIZE) buffer->head = 0;
	}
}


int main (void)
{
   UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry
   UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

   UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
   UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register

   UCSR0B |= (1 << RXCIE0) | _BV(UDRIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
   sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed

	DDRB |= _BV(PORTB4) | _BV(PORTB0) | _BV(PORTB1);

   for (;;) // Loop forever
   {
   
		PORTB ^= _BV(PORTB4);
   }   
}

ISR(USART0_RX_vect)
{
	char data = UDR0;

	_buffer_put(&rx_buffer, data);
	
	//Signal that there is data available; the UDRE interrupt will fire.
	UCSR0B |= _BV(UDRIE0);	
}

ISR(USART0_UDRE_vect){
		if (_buffer_count(&rx_buffer)){
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
