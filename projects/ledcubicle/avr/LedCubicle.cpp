#include "LedCubicle.h"

#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#include "Matrix.h"

#include "Life.h"
#include "Plasma.h"

using namespace digitalcave;

volatile uint8_t timer = 0;
Matrix matrix = Matrix();
uint8_t baseColor = 0; // 0 = green, 1 = yellow, 2 = red

const uint8_t wj[] PROGMEM = { 0x88, 0x0, 0x0, 0x88, 0x0, 0x0, 0x8b, 0x36, 0x4c, 0x88, 0xa4, 0xaa, 0xa9, 0xa4, 0xea, 0xaa, 0xa4, 0x8a, 0x51, 0xa4, 0x6a, 0x0, 0x0, 0x0, 0xe0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x2d, 0x36, 0xd3, 0x25, 0xa4, 0x9a, 0x2d, 0xb6, 0xdb, 0x2d, 0x92, 0x99, 0xcd, 0xb6, 0xdb, 0x0, 0x0, 0x0 };

int main() {
	TCCR0A = 0;
	TCCR0B = _BV(CS00) | _BV(CS02); // clk / 1024
	OCR0A = 180; // 180 * 256 = 3 seconds
	
	srandom(0);

	while (1) {
		matrix.setDepth(0); // 4-bit
		matrix.setColor(0,0);
		matrix.rectangle(0,0,MATRIX_WIDTH,MATRIX_HEIGHT,DRAW_FILLED);
		
		
		matrix.flush();
		
		
		
		baseColor++;
		baseColor %= 3;
		
		// TODO implement a script that loops through the name bitmap, life, plasma

		_delay_ms(127);
	}
}


ISR(TIMER0_COMPA_vect) {
	timer++;
}