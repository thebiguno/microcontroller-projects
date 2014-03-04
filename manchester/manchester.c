/*
 * A manchester signal generator.
 */

#include "manchester.h"
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile uint8_t *port;
static volatile uint8_t *ddr;
static uint8_t pin;

static uint8_t lower;	// 1/4
static uint8_t mid;   	// 3/4
static uint8_t upper;	// 5/4

static uint8_t l; 		// the message length as indicated by byte 1 of the packet 
static uint8_t dLength;			// the maximum number of bytes the caller can receive
static volatile uint8_t buf;	// one byte buffer
static volatile uint8_t dPos;	// the position in the data buffer
static volatile uint8_t *d;		// reference to the buffer passed in by the caller
static volatile uint8_t chk;	// running checksum

// 0: msg ready
// 1: signal state (where in the bit)
// 2-4: bit position in current byte (where in the byte)
// 5-6: byte position in packet (where in the packet)
// 7: unused
static volatile uint8_t status;


/*
 * Using timer 0 the baud rate can range from 300 to 62500 at 16 MHz
 */
void manchester_tx_init(volatile uint8_t *port, uint8_t pin, uint16_t baud){
	TCCR0A = 0x0; 				// normal mode
	TCCR0B |= _BV(CS02);        // F_CPU / 256 prescaler
	OCR0A = F_CPU / 256 / baud; // compare value
	TIMSK0 = _BV(OCIE0A);		// enable compare interrupt
	
	sei();
}

void manchester_rx_init(){
	TCCR0A = 0x0; 				// normal mode
	TCCR0B |= _BV(CS02);        // F_CPU / 256 prescaler
	
	uint8_t half = F_CPU / 255 / baud;
	lower = half / 2;
	mid = lower + half;
	upper = mid + half;

	EICRA |= _BV(ISC00);		// any logical change on int0
	EIMSK |= _BV(INT0);			// enable external interrupts on int0
	
	sei();
}

void manchester_write_c(char data){
	//Nop loop to wait until last transmission has completed
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = data;
}

/*
 * buf is up to 255 bytes to receive data into
 * length is maximum number of bytes that can be written to buf
 * status is a bit mask where:
 *   bit 0 is the last 
 *   0x00 to 0

 *   bit 0 set indicates that a message has been received and can be read; the receiver clears this bit to indicate receipt
 *   bit 1 set indicates that the receiver is in an error state
 */
void manchester_read(uint8_t *data, uint8_t length, char *mbox) {
	dLength = length;
	d = data;
	m = mbox;
}

inline void readBit() {
	uint8_t b = ((status >> 2) & 0x07);
	if (PORTD & _BV(PIND6)) {
		// transition from low to high = 1
		buf |= _BV(b);
	} else {
		// transition from high to low = 0
		buf &= ~_BV(b);
	}
	
	status &= 0xE3;
	if (b == 7) {
		uint8_t pos = (status >> 5) & 0x03;
		if (pos == 0) {
			// preamble
			if (buf == 0xFE) {
				// valid
				status |= _BV(5); // set position to 1
			} else {
				// invalid
				status = 0;
			}
		} else if (pos == 1) {
			if (dLength < buf) {
				// message is too long
				status = 0;
			} else {
				l = buf;
				chk ^= buf;
				status ^= _BV(5) | _BV(6); // set position to 2
			}
		} else if (pos == 2) {
			// somewhere in the data
			d[dPos] = buf;
			chk ^= buf;
			dPos++;
			
			if (dPos >= l) {
				status |= _BV(5); // set position to 3
			}
		} else {
			// checksum
			if ((chk ^ buf) == 0) {
				// valid
				status = 1;
			} else {
				// invalid
				status = 0;
			}
		}
	} else {
		// increment the bit counter
		status |= (b++ << 2);
	}
}

ISR(PCINT0_vect)
{
	uint8_t ck = TCNT0;
	if (ck < lower) {
		// error, there should never be a transition this soon
		//status |= _BV(7);
		status &= ~_BV(1); // clear signal state bit
	} else if (ck < mid) {
		// one clock pulse between transitions

		if (status & _BV(1)) {
			readBit();
		}
		status ^= _BV(1); // toggle signal state bit
	} else if (ck < upper) {
		if ((status & _BV(1)) == 0) {
			readBit();
		} else {
			// error
			status &= ~_BV(1); // clear signal state bit
		}
	} else {
		// error
	}

	if (PORTD & _BV(PIND6)) {
		// high
	} else {
		// low
	}
	
	TCNT0 = 0x00;
}



