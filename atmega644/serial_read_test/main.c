#include <avr/io.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

int main (void)
{
   UCSR0B |= (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry
   UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

   UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
   UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register

   UCSR0B |= (1 << RXCIE0); // Enable the USART Recieve Complete interrupt (USART_RXC)
   sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed

   for (;;) // Loop forever
   {
         // Do nothing - echoing is handled by the ISR instead of in the main loop
   }   
}

ISR(USART0_RX_vect)
{
   char ReceivedByte;
   ReceivedByte = UDR0; // Fetch the recieved byte value into the variable "ByteReceived"
   UDR0 = ReceivedByte; // Echo back the received byte back to the computer
}
