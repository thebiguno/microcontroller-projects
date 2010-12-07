#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define BUFFER_SIZE 256

char buf[BUFFER_SIZE];
int head = 0;
int tail = 0;

int available_in_buffer(void)
{
	// This is the hardest part,
	// calculating the size of data in buffer.
	return ((head - tail ) % BUFFER_SIZE);
}

inline char get_from_buffer(void)
{
	char data = buf[tail];
	if (++tail >= BUFFER_SIZE) tail = 0;
	return data;
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

   		UDR0 = 'W';

   for (;;) // Loop forever
   {
   
		while (available_in_buffer())
		{
			
//			_delay_ms(1);
		}
		PORTB &= ~_BV(PINB0);
		
		PORTB ^= _BV(PORTB4);
   }   
}

ISR(USART0_RX_vect)
{
	char b = UDR0;
//	UDR0 = b;
	if ((head + 1) % BUFFER_SIZE != tail)
	{
		buf[head] = b;
		if (++head >= BUFFER_SIZE) head = 0;
	}
	
	//Signal that there is data available; the UDRE interrupt will fire.
	UCSR0B |= _BV(UDRIE0);	
}

ISR(USART0_UDRE_vect){
//			PORTB |= _BV(PINB0);
		if (available_in_buffer()){

			char data = get_from_buffer();
//			while ( !( UCSR0A & (1<<TXC0)) )
//				;
							
			UDR0 = data;
		}
		else {
			//Once the ring buffer is empty (i.e. head == tail), we disable
			// USART Data Register Empty interrupts until someone write to
			// the tx buffer again.
			UCSR0B &= ~_BV(UDRIE0);			
		}

}
