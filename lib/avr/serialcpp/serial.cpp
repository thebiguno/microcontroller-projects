/*
 * An OO implementation of serial library.  You can use the compile-time flags SERIAL_DISABLE_RX 
 * and SERIAL_DISABLE_TX to control whether the RX / TX code is excluded to save program space.
 */

#include "serial.h"
#include <avr/interrupt.h>

static Ring *rx0;
static Ring *tx0;
static Ring *rx1;
static Ring *tx1;

Serial::Serial(uint32_t baud, uint8_t usartNumber, uint8_t flags){
	if (usartNumber == 0){
		regBaudLow = &UBRR0L;
		regBaudHigh = &UBRR0H;
		regControlStatusA = &UCSR0A;
		regControlStatusB = &UCSR0B;
		regControlStatusC = &UCSR0C;
		regData = &UDR0;
#ifndef SERIAL_DISABLE_RX
		*rx0 = Ring(0xFF);
		rx = rx0;
#endif
#ifndef SERIAL_DISABLE_TX
		*tx0 = Ring(0xFF);
		tx = tx0;
#endif
	}
	else if (usartNumber == 1){
		regBaudLow = &UBRR1L;
		regBaudHigh = &UBRR1H;
		regControlStatusA = &UCSR1A;
		regControlStatusB = &UCSR1B;
		regControlStatusC = &UCSR1C;
		regData = &UDR1;
	}

	//Set baud rate
	uint16_t calculated_baud = (F_CPU / 16 / baud) - 1;
	*regBaudHigh = calculated_baud >> 8;
	*regBaudLow = calculated_baud & 0xFF;

	//Make sure 2x and multi processor comm modes are off
	*regControlStatusA = 0x00;
	
	//Set frame format to 8 data bits, even parity, 1 stop bit (default):
	*regControlStatusC = _BV(UCSZ01) | _BV(UCSZ00) | _BV(UPM01);
	
	//Enable Rx / Tx if they are not disabled at compile time
#ifndef SERIAL_DISABLE_RX
	*regControlStatusB |= _BV(RXEN0);
	
	if (flags | SERIAL_INTERRUPT_RX){
		//Enable RX interrupts
		*regControlStatusB |= _BV(RXCIE0);

	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
		sei();
#endif	
	}
#endif
#ifndef SERIAL_DISABLE_TX
	*regControlStatusB |= _BV(TXEN0);
	//_serial_init_tx();
#endif
}






/////////// TX functions / interrupts ///////////////

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
#error You must define USART vectors for your chip!  Please verify that MMCU is set correctly, and that there is a matching vector definition in serial_asynch_tx.c
void error2() {
#endif
	if (!tx0->isEmpty()){
		UDR0 = tx0->get();
	}
	else {
		//Once the ring buffer is empty (i.e. head == tail), we disable
		// USART Data Register Empty interrupts until someone writes to
		// the tx buffer again.
		UCSR0B &= ~_BV(UDRIE0);
	}
}