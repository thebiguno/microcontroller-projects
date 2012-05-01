#include "lib/shiftcpp/shift.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
// #include "lib/serial/serial.h"

static Shift shift(2);
static uint8_t red[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t grn[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t r1[8] = { 0x10, 0x00, 0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0x0F };
static uint8_t r0[8] = { 0x10, 0xFF, 0x00, 0xFF, 0x0F, 0x0F, 0x00, 0x0F };
static uint8_t g1[8] = { 0x08, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0xFF, 0xFF };
static uint8_t g0[8] = { 0x08, 0xF0, 0x00, 0xF0, 0x0F, 0xFF, 0x00, 0xFF };
static volatile uint8_t row;

#define B_BITMASK 0x03
#define D_BITMASK 0xFC

uint8_t data[2];
uint8_t dc;

void translate(uint8_t red, uint8_t green, uint8_t *data) {
	data[0] = 0x00;
	data[1] = 0x00;
	
	if (red & _BV(0)) data[1] |= _BV(0);
	if (green & _BV(0)) data[1] |= _BV(1);
	
	if (red & _BV(1)) data[1] |= _BV(6);
	if (green & _BV(1)) data[1] |= _BV(7);
	
	if (red & _BV(2)) data[0] |= _BV(0);
	if (green & _BV(2)) data[0] |= _BV(1);
	
	if (red & _BV(3)) data[0] |= _BV(6);
	if (green & _BV(3)) data[0] |= _BV(7);
	
	if (red & _BV(4)) data[1] |= _BV(3);
	if (green & _BV(4)) data[1] |= _BV(2);
	
	if (red & _BV(5)) data[1] |= _BV(5);
	if (green & _BV(5)) data[1] |= _BV(4);
	
	if (red & _BV(6)) data[0] |= _BV(3);
	if (green & _BV(6)) data[0] |= _BV(2);
	
	if (red & _BV(7)) data[0] |= _BV(5);
	if (green & _BV(7)) data[0] |= _BV(4);
}

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

void set_dc() {
	uint8_t m = dc % 9;
	if (m == 1) {
		for (uint8_t i = 0; i < 8; i++) {
			red[i] = r1[i] | r0[i];
			grn[i] = g1[i] | g0[i];
		}
	} else if (m == 3 || m == 5) {
		for (uint8_t i = 0; i < 8; i++) {
			red[i] = r1[i];
			grn[i] = g1[i];
		}
	} else {
		for (uint8_t i = 0; i < 8; i++) {
			red[i] = r1[i] & r0[i];
			grn[i] = g1[i] & g0[i];
		}
	}
	dc++;
	if (dc == 9) dc = 0;
}

void callback() {
	set_row(-1);
	PORTC &= ~_BV(PC0);
 	PORTC |= _BV(PC0);
	set_row(row++);

	if (row == 8) row = 0;
}

int main (void){
	TCCR0A = 0x0;					// output a = normal mode; output b = normal mode; waveform generation = normal
	TCCR0B |= _BV(CS02);			// clock select = CLK / 256 (prescaler)
	
	OCR0A = F_CPU / 256 / 4000;		// interrupt A = every 1/4 millisecond
	
	TIMSK0 = _BV(OCIE0A);
	
	// set up PORTB and PORTD as row drivers
	DDRD |= _BV(DDD2) | _BV(DDD3) | _BV(DDD4) | _BV(DDD5) | _BV(DDD6) | _BV(DDD7);
	DDRB |= _BV(DDB0) | _BV(DDB1);

	DDRC |= _BV(DDC0);

	shift.setCallback(callback);
	
	// serial_init_b(9600);

	sei();

	while (1) {
		;
	}
}

ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0;						// reset counter to zero

	if (shift.cts()) {
		set_dc();
		translate(red[row], grn[row], data);
		shift.shift(data);
	}
}
