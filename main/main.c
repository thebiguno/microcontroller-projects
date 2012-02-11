#include "main.h"

int main() {
	
	segment_init(&PORT_SEGMENT_DATA, PIN_SEGMENT_DATA, &PORT_SEGMENT_CLOCK, PIN_SEGMENT_CLOCK, &PORT_SEGMENT_LATCH, PIN_SEGMENT_LATCH);
	matrix_init(&PORT_MATRIX_DATA, PIN_MATRIX_DATA, &PORT_MATRIX_CLOCK, PIN_MATRIX_CLOCK, &PORT_MATRIX_LATCH, PIN_MATRIX_LATCH);
	
	
	uint8_t MATRIX_GRN[8] = { 0x00,0x00,0x00,0x00,0x55,0xAA,0x55,0xAA };
	uint8_t MATRIX_RED[8] = { 0xAA,0x55,0xAA,0x55,0x00,0x00,0x00,0x00 };
	uint8_t MATRIX_OFF[8] = { 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0 };
	uint8_t MATRIX_ON[8] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
	while(1) {
		char c[] = "8888";
		segment_draw(c, 0xF);
		
		matrix_draw(&MATRIX_RED[0],&MATRIX_GRN[0]);
	}
}