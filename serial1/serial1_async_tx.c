/*
 * An asynchronous implementation of the tx portion of the serial library.  This will let you submit a bunch
 * of data to a buffer, and let the AVR send it using interrupts.  This results in shorter
 * send times (as the write methods do not block), but there are downsides.  It a) causes code 
 * to be run in interrupts (which may interere with timing-critical code), b) adds substantially
 * to the total SRAM used by a program (defaults to a 64 byte buffer for transmitting and receiving, 
 * although this buffer size can be configured at compile time), c) introduces the possibility 
 * that data will be partially unsent, if the program tries to write too much at a time and overflows 
 * the buffer, and d) for single byte messages which are not sent continuously, there is actually a 
 * performance penalty as there is an extra layer of abstraction (the buffer) in between the hardware 
 * and the sending program.
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
#include "../Ring/Ring.h"
#include <avr/interrupt.h>

static Ring tx_buffer(SERIAL_BUFFER_SIZE);

void _serial1_init_tx(){
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void serial1_write_c(char data){
	//Disable UCSR interrupts temporarily to avoid clobbering the buffer
	UCSR0B &= ~_BV(UDRIE1);
	tx_buffer.put(data);
	
	//Signal that there is data available; the UDRE interrupt will fire.
	UCSR1B |= _BV(UDRIE1);
}

void serial1_write_s(char *data){
	while (*data){
		serial1_write_c(*data++);
	}
}

ISR(USART1_UDRE_vect){
	if (!tx_buffer.isEmpty()){
		UDR1 = tx_buffer.get();
	}
	else {
		//Once the ring buffer is empty (i.e. head == tail), we disable
		// USART Data Register Empty interrupts until someone writes to
		// the tx buffer again.
		UCSR1B &= ~_BV(UDRIE1);
	}
}
