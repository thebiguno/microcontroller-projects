/*
 * High Voltage Chip rescue code.  Adapted from http://denki.world3.net/avr_rescue.html, which 
 * in turn was adapted from http://mightyohm.com/blog/2008/09/arduino-based-avr-high-voltage-programmer/.
 *
 * I copied this code with the intent of drawing out simple methods to connect each of the 
 * chips that I have bricked.
 *
 * Connections: 
 * 			Host AVR 		Target AVRs
 * Def		(Mega168/RBBB)	Mega644(P)(A)	
 * _________________________________________________________________________
 * VCC		PB0/D8			VCC/AVCC
 * BS1		PB1/D9			PD4
 * WR		PB2/D10			PD3
 * OE		PB3/D11			PD2
 * RDY		PB4/D12			PD1
 * XA0		PB5/D13			PD5
 * RST		PC0/A0			12V to Reset
 * BS2		PC2/A2			PA0
 * XTAL1	PC3/A3			XTAL1
 * XA1		PC4/A4			PD6
 * PAGEL	PC5/A5			PD7
 *
 * DATA		PD0..7/D0..D7	PB0..7
 *
 * Fuse Settings			L 0x42 H 0x99
 * 
 * Button	PC1/A1 (to GND)
 */

#include <avr/io.h>
#include <util/delay.h>

/* For fuse values, see
http://www.engbedded.com/cgi-bin/fc.cgi */
#define	HFUSE	0x99
#define	LFUSE	0x42

/*
#define  DATA    PORTD // PORTD = Arduino Digital pins 0-7

#define  DATAD   DDRD  // Data direction register for DATA port

#define  VCC     8		PB0

#define  RDY     12     PB4 // RDY/!BSY signal from target

#define  OE      11		PB3

#define  WR      10		PB2

#define  BS1     9		PB1

#define  XA0     13		PB5

#define  XA1     18    	PC4	// Analog inputs 0-5 can be addressed as

#define  PAGEL   19    	PC5	// digital outputs 14-19

#define  RST     14    	PC0	// Output to level shifter for !RESET

#define  BS2     16		PC2

#define  XTAL1   17		PC3


#define  BUTTON  15    	PC1	// Run button
#define  LED     0
*/

// PORTB
#define	_VCC	_BV(0)
#define	_RDY	_BV(4)
#define	_OE		_BV(3)
#define	_WR		_BV(2)
#define	_BS1	_BV(1)
#define	_XA0	_BV(5)

// PORTC
#define	_XA1	_BV(4)
#define	_PAGEL	_BV(5)
//PC0 is connected to transistor to enable 12V reset.
#define	_RST	_BV(0)
#define	_BS2	_BV(2)
#define	_XTAL1	_BV(3)

//Connected to GND via push button
#define	_BUTTON	_BV(1)

void HardwareInit() {
	DDRB	= ~(_RDY);			// all outputs except RDY (PB4)
	PORTB	= 0x00;				// no pull-ups

	DDRC	= ~(_BUTTON);		// all outputs except BUTTON (PC1)
	PORTC	= (_BUTTON);		// pull-up on button

	DDRD	= 0xff;				// all outputs
	PORTD	= 0x00;
}

void sendcmd(unsigned char command) {
	PORTC |= _XA1;
	PORTB &= ~(_XA0|_BS1);
	
	PORTD = command;

	PORTC |= _XTAL1;
	_delay_ms(1);
	PORTC &= ~(_XTAL1);
	_delay_ms(1);
}

void writefuse(unsigned char fuse, char highbyte) {
	PORTC &= ~(_XA1);
	PORTB |= _XA0;
	_delay_ms(100);

	PORTD = fuse;

	PORTC |= _XTAL1;
	_delay_ms(100);
	PORTC &= ~(_XTAL1);

	if (highbyte) {
		PORTC &= ~(_BS2);
		PORTB |= _BS1;	
	}

	PORTB &= ~(_WR);
	_delay_ms(100);
	PORTB |= _WR;
	
	//Wait for RDY to go high
	while (!(PINB & _RDY)){}
	
	if (highbyte) {
		PORTC &= ~(_BS2);
		PORTB &= ~(_BS1);
	}

	
	_delay_ms(100);
}

int main() {
	while(1) {
		HardwareInit();
		
		_delay_ms(1000);	// allow button to be released from last time

		while (PINC & _BUTTON) {}	// wait for button

		//From ATMega644PA datasheet, section 26.7.1:
		//The following algorithm puts the device in parallel programming mode:
		//1.	Apply 4.5 - 5.5V between VCC and GND.
		PORTB |= _VCC;
		PORTB |= _WR | _OE; //These are active low pins, so set them high
		
		_delay_ms(10);
		
		//2.	Set RESET to "0" and toggle XTAL1 at least six times.
		PORTC &= ~_RST;
		for (uint8_t i = 0; i < 10; i++){
			PORTC |= _XTAL1;
			_delay_ms(1);
			PORTC &= ~_XTAL1;
			_delay_ms(1);
		}
		
		//3.	Set the Prog_enable pins listed in Table 26-11 on page 300 to “0000” and wait at least 100 ns.
		PORTB &= ~(_PAGEL | _XA0);
		PORTC &= ~(_XA1 | _BS1);
		_delay_ms(100);
		
		//4.	Apply 11.5 - 12.5V to RESET. Any activity on Prog_enable pins within 100 ns after +12V
		//		has been applied to RESET, will cause the device to fail entering programming mode. 
		
		PORTC |= _RST;
		
		//5.	Wait at least 50 μs before sending a new command.
		_delay_ms(10);


/* Old version
		// Initialize pins to enter programming mode
		PORTC &= ~(_PAGEL|_XA1|_BS2);
		PORTB &= ~(_XA0|_BS1);
	
		// Enter programming mode
		PORTB |= _VCC|_WR|_OE;
		_delay_ms(100);
		PORTC &= ~(_RST);
		_delay_ms(100);
*/

		// Program HFUSE
//		sendcmd(0b01000000);
//		writefuse(HFUSE, 1);

		// Program LFUSE
		sendcmd(0b01000000);
  		
		writefuse(LFUSE, 0);

		// Exit programming mode
/* Old version
		PORTC |= _RST;

		// Turn off outputs
		PORTD = 0x00;
		PORTB &= ~(_VCC|_WR|_OE|_XA0|_BS1);
		PORTC &= ~(_PAGEL|_XA1|_BS2);
*/
	}
}
