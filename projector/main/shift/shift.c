#include "shift.h"

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//static volatile uint8_t _digit;

void shift_init(){
	DDRC = (1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3);

	// Set MOSI and SCK output, all others input
	DDRB = (1<<PB3) | (1<<PB5);
	
	/* Enable SPI, Master, Interrupt, set clock rate fck/16 */
	// SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPIE) | (1<<SPR0);
}

void shift_set(uint8_t segments, uint8_t digit) {
	PORTC = 0x00;

	// if(!(SPSR & (1<<SPIF)));
	// _digit = digit; // remember the second byte
	// SPDR = segments; // shift the first byte
	
	for (int i = 0; i < 8; i++){
				//Clear the pin first...
				PORTB &= ~_BV(PB3);
				//... then set the bit (if appropriate).  We could probably
				// do this in one step, but this is more clear, plus speed is 
				// (probably) not critical here.
				PORTB |= (((segments >> (7 - i)) & 0x1) << PB3);
				// _delay_ms(1);
				
				//Pulse clock to shift in
				PORTB &= ~_BV(PB5);
				// _delay_ms(1);
				PORTB |= _BV(PB5);
			}
		
			PORTC = _BV(digit);
}

/////////// interrupts ///////////////

// ISR(SPI_STC_vect) {
// 	if (_digit != 0) {
// 		SPDR = _digit; // shift the second byte
// 		_digit = 0;
// 	}
// } 
