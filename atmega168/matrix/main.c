#include "lib/shiftcpp/shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
// #include "lib/serial/serial.h"

static Shift shift(2);
static uint8_t a[8] = { 0x00, 0x96, 0xFF, 0x96, 0x00, 0x96, 0xFF, 0x96 };
static uint8_t b[8] = { 0xFF, 0x69, 0x00, 0x69, 0xFF, 0x69, 0x00, 0x69 };
static volatile uint8_t row;

#define B_BITMASK 0x03
#define D_BITMASK 0xFC

uint8_t data[2];
char temp[32];

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

void callback() {
	set_row(-1);
	PORTC &= ~_BV(PC0);
 	PORTC |= _BV(PC0);
	set_row(row++);
	if (row < 8) {
		data[0] = a[row];
		data[1] = b[row];
		shift.shift(data);
	}
}

int main (void){
	DDRD |= _BV(DDD2) | _BV(DDD3) | _BV(DDD4) | _BV(DDD5) | _BV(DDD6) | _BV(DDD7);
	DDRB |= _BV(DDB0) | _BV(DDB1);

	DDRC |= _BV(DDC0);

	// shift.setLatch(&PORTC,PINC0);
	shift.setCallback(callback);
	
	// serial_init_b(9600);
	sei();

	row = 8;
	while (1) {
		if (row == 8) {
			row = 0;
			// shift in row 0
			data[0] = a[row];
			data[1] = b[row];
			shift.shift(data);
		}
		// if (ov++ == 0) {
		// 	data[0]++;
		// 	data[1]++;
		// }
		 // _delay_ms(2);
	}
}
