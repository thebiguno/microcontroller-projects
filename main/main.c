#include "main.h"

int main() {
	
	segment_init(&PORT_SEGMENT_DATA, PIN_SEGMENT_DATA, &PORT_SEGMENT_CLOCK, PIN_SEGMENT_CLOCK, &PORT_SEGMENT_LATCH, PIN_SEGMENT_LATCH);
	matrix_init(&PORT_MATRIX_DATA, PIN_MATRIX_DATA, &PORT_MATRIX_CLOCK, PIN_MATRIX_CLOCK, &PORT_MATRIX_LATCH, PIN_MATRIX_LATCH);
	
	while(1) {
		char c[] = "1234";
		for (int i = 0; i < 1; i++) {
			c[0] = 'F';
			segment_draw(c, 0x0);
		}
	}
}