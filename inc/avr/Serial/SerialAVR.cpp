#include "SerialAVR.h"
#include <util/delay.h>
using namespace digitalcave;

SerialAVR::SerialAVR(uint32_t baud, uint8_t dataBits, uint8_t parity, uint8_t stopBits, uint8_t serialPort, uint8_t bufferSize):
	rxBuffer(bufferSize) {
	if (serialPort == 0){
#ifdef UDR0
		UBRRH = &UBRR0H;
		UBRRL = &UBRR0L;
		UCSRA = &UCSR0A;
		UCSRB = &UCSR0B;
		UCSRC = &UCSR0C;
		UDR = &UDR0;

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
	uint16_t calculated_baud = (F_CPU / 4 / baud - 1) / 2;
	*UCSRA |= _BV(U2X1);
	if (((F_CPU == 16000000UL) && (baud == 57600)) || (calculated_baud > 4095)){
		*UCSRA &= ~_BV(U2X1);
		calculated_baud = (F_CPU / 8 / baud - 1) / 2;
	}
	*UBRRH = calculated_baud >> 8;
	*UBRRL = calculated_baud & 0xFF;

	//Make sure multi processor comm mode is off
	*UCSRA &= ~(_BV(MPCM));

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

uint8_t SerialAVR::read(uint8_t *c){
	*UCSRB &= ~_BV(RXCIE); //Temporarily disable RX interrupts so we don't get interrupted
	if (!rxBuffer.isEmpty()){
		if (rxBuffer.read(c)){
			*UCSRB |= _BV(RXCIE); //Re-enable interrupts
			return 1;
		}
	}
	*UCSRB |= _BV(RXCIE); //Re-enable interrupts
	return 0;
}

uint8_t SerialAVR::write(uint8_t b){
	//Nop loop to wait until last transmission has completed
	while (!(*UCSRA & _BV(UDRE)));
	*UDR = b;
	return 1;
}

void SerialAVR::isr(){
	rxBuffer.write(*UDR);
}
