#include "Serial_AVR.h"

using namespace digitalcave;

/*
 * You need to put an ISR into your own code somewhere to handle incoming characters and pass them to the 
 * serial object.  To keep the library code separate and clean, we can't put this into the library itself.
 */
//ISR(USART1_RX_vect){
//	//Be sure to pass your serial object instance the correct data; i.e. UDR0 for serial port 0, UDR1 
//	// for serial port 1, etc.
//	serial.handleISR(UDR0);
//}

Serial_AVR::Serial_AVR(uint32_t baud, uint8_t dataBits, uint8_t parity, uint8_t stopBits, uint8_t serialPort):
	rxBuffer(SERIAL_BUFFER_SIZE) {
	if (serialPort == 0){
#ifdef UDR0
		UBRRH = UBRR0H;
		UBRRL = UBRR0L;
		UCSRA = UCSR0A;
		UCSRB = UCSR0B;
		UCSRC = UCSR0C;
		UDR = UDR0;
		
		UDRE = UDRE0;
		UCSZ0 = UCSZ00;
		UCSZ1 = UCSZ01;
		UCSZ2 = UCSZ02;
		RXEN = RXEN0;
		TXEN = TXEN0;
		RXCIE = RXCIE0;
		U2X = U2X0;
		MPCM = MPCM0;
		UPM0 = UPM00;
		USBS = USBS0;
#endif
	}
	else if (serialPort == 1){
#ifdef UDR1
		UBRRH = &UBRR1H;
		UBRRL = &UBRR1L;
		UCSRA = &UCSR1A;
		UCSRB = &UCSR1B;
		UCSRC = &UCSR1C;
		UDR = &UDR1;
		
		UDRE = UDRE1;
		UCSZ0 = UCSZ10;
		UCSZ1 = UCSZ11;
		UCSZ2 = UCSZ12;
		RXEN = RXEN1;
		TXEN = TXEN1;
		RXCIE = RXCIE1;
		
		U2X = U2X1;
		MPCM = MPCM1;
		UPM0 = UPM10;
		USBS = USBS1;
#endif
	}
	else {
		//TODO Support other serial ports if needed
		return;
	}
	
	
	//Set baud rate
	uint16_t calculated_baud = (F_CPU / 16 / baud) - 1;
	*UBRRH = calculated_baud >> 8;
	*UBRRL = calculated_baud & 0xFF;

	//Make sure 2x and multi processor comm modes are off
	*UCSRA &= ~(_BV(U2X) | _BV(MPCM));
	
	//Calculate frame format
	//Init to 0; we populate this later
	*UCSRC = 0x0;
	
	//Data bits
	if (dataBits == 9){
		//9 bits use an extra bit in register UCSR0B
		*UCSRB |= _BV(UCSZ2);
		*UCSRC |= _BV(UCSZ1) | _BV(UCSZ0);
	}
	else {
		*UCSRC |= ((dataBits - 5) << UCSZ0);
	}
	
	//Parity, Stop bits
	*UCSRC |= (parity << UPM0) | ((stopBits - 1) << USBS);
	
	//Enable Rx (interrupt) / Tx (blocking)
	*UCSRB |= _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
}

uint8_t Serial_AVR::read(uint8_t *c){
	*UCSRB &= ~_BV(RXCIE); //Temporarily disable RX interrupts so we don't get interrupted
	if (!rxBuffer.isEmpty()){
		*c = rxBuffer.get();
		*UCSRB |= _BV(RXCIE); //Re-enable interrupts
		return 1;
	}
	*UCSRB |= _BV(RXCIE); //Re-enable interrupts
	return 0;
}

void Serial_AVR::write(uint8_t b){
	//Nop loop to wait until last transmission has completed
	while (!(*UCSRA & _BV(UDRE)));
	*UDR = b;
}

void Serial_AVR::isr(){
	if (!this->rxBuffer.isFull()){
		this->rxBuffer.put(*UDR);
	}
}