#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/Shift/Shift.h"

int main (void){
	Shift shift(&PORTB, PORTB0, &PORTB, PORTB1);
	matrix_init();
	
	uint8_t incoming;

	while (1) {
		//Loop until we get the sync frame
//		while(incoming != 0x42) shift.receive(&incoming, 1);
		
		//Read the data directly into the working buffer
		shift.receive(matrix_get_working_buffer(), MATRIX_WIDTH * (MATRIX_HEIGHT >> 1));
		
		matrix_flush();
	}
}
