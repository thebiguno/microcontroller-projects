#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/twi/twi.h"

#include <util/delay.h>

void flushBufferCallback(uint8_t* data, uint16_t length){
	if (length == MATRIX_WIDTH + 1){
		uint8_t *buffer = matrix_get_display_buffer() + (MATRIX_WIDTH * data[0]);
		for (uint8_t i = 0; i < (length - 1); i++){
			buffer[i] = data[i + 1];
		}
	}
}

int main (void){
	twi_init();
	twi_set_slave_address(42);
	twi_attach_slave_rx_event(flushBufferCallback);
	matrix_init();
	
	while (1) {
		//Nothing to see here... all the work is done in ISRs
	}
}
