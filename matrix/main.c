#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/TwoWire/TwoWire.h"

void flushBufferCallback(int bytesRecieved){
	if (Wire.available() == MATRIX_WIDTH * (MATRIX_HEIGHT >> 1)){
		uint8_t *working_buffer = matrix_get_working_buffer();
		for (uint8_t y = 0; y < (MATRIX_HEIGHT >> 1); y++){
			for (uint8_t x = 0; x < MATRIX_WIDTH; x++){
				working_buffer[x + MATRIX_WIDTH * y] = Wire.receive();
			}
		}
	}
	
	matrix_flush();
}

int main (void){
	Wire.begin(42);
	Wire.onReceive(flushBufferCallback);
	matrix_init();
	
	while (1) {
		//Nothing to see here... all the work is done in ISRs
	}
}
