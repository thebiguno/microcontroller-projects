#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/twi/twi.h"

#include <util/delay.h>

void flushBufferCallback(uint8_t* data, uint16_t length){
	//Do nothing... the slave rx buffer *is* the display buffer
}

int main (void){
	twi_init();
	twi_set_slave_address(42);
	twi_attach_slave_rx_event(flushBufferCallback);
	twi_set_rx_buffer(matrix_get_display_buffer());
	matrix_init();
	
	while (1) {
		//Nothing to see here... all the work is done in ISRs
	}
}
