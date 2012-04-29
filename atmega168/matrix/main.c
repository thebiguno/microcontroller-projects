#include "lib/shiftcpp/shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>

static Shift shift(2);
static uint8_t a[8] = { 0x69, 0x96, 0x69, 0x96, 0x69, 0x96, 0x69, 0x96 };
static uint8_t b[8] = { 0x96, 0x69, 0x96, 0x69, 0x96, 0x69, 0x96, 0x69 };
static volatile uint8_t row;

#define B_BITMASK 0x03
#define D_BITMASK 0xFC


// void translate(uint8_t red, uint8_t green) {
// 	
// }

void set_row(uint8_t r) {
	switch(r) {
		case 0: 
			PORTB = (PORTB & ~B_BITMASK) | _BV(0);
			PORTD = (PORTD & ~D_BITMASK);
			break;
		case 1:
			PORTB = (PORTB & ~B_BITMASK) | _BV(1);
			PORTD = (PORTD & ~D_BITMASK);
			break;
		case 2:
			PORTB = (PORTB & ~B_BITMASK);
			PORTD = (PORTD & ~D_BITMASK) | _BV(2);
			break;
		case 3:
			PORTB = (PORTB & ~B_BITMASK);
			PORTD = (PORTD & ~D_BITMASK) | _BV(3);
			break;
		case 4:
			PORTB = (PORTB & ~B_BITMASK);
			PORTD = (PORTD & ~D_BITMASK) | _BV(4);
			break;
		case 5:
			PORTB = (PORTB & ~B_BITMASK);
			PORTD = (PORTD & ~D_BITMASK) | _BV(5);
			break;
		case 6:
			PORTB = (PORTB & ~B_BITMASK);
			PORTD = (PORTD & ~D_BITMASK) | _BV(6);
			break;
		case 7:
			PORTB = (PORTB & ~B_BITMASK);
			PORTD = (PORTD & ~D_BITMASK) | _BV(7);
			break;
		default:
			PORTB = (PORTB & ~B_BITMASK);
			PORTD = (PORTD & ~D_BITMASK);
			break;
	}
}

void go() {
	// SPCR |= _BV(MSTR); // this shouldn't be required but something keeps clearing MSTR
	// set_row(-1);
	// PORTB &= ~_BV(PB2);
	// _delay_us(5);
	// PORTB |= _BV(PB2);
	set_row(row);
	if (row < 8) {
		uint8_t data[2];
		data[0] = 0x96; //a[row];
		data[1] = 0x96; //b[row];
		shift.shift(data);
		row++;
	}
	if (row == 8) row = 0;
}

int main (void){
	DDRD = _BV(DDD2) | _BV(DDD3) | _BV(DDD4) | _BV(DDD5) | _BV(DDD6) | _BV(DDD7);
	DDRB = _BV(DDB0) | _BV(DDB1);


	shift.setLatch(&PORTB,PINB2); // use SS as the latch clock
	shift.setCallback(go);

	uint16_t ov = 0;

	go();
	while (1) {
		;
		// row = 0;
		
		// if (ov++ == 0) {
		// 	data[0]++;
		// 	data[1]++;
		// }
		// _delay_ms(2);
	}
}
