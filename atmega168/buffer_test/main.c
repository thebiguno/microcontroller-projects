#define BUFFER_HEIGHT_SHIFT		2

#include "lib/draw/buffer/buffer.h"
#include "lib/twi/twi.h"

#include <util/delay.h>

int main (void){
	twi_init();
	twi_set_master_buffer(get_buffer());
	get_buffer()[0] = 0x06;
	
	uint16_t i = 0;
	uint8_t v = 0xFF;

	for (uint16_t i = 0; i < BUFFER_LENGTH + BUFFER_PROLOG; i++){
		get_buffer()[i + BUFFER_PROLOG] = i;
	}

	while (1) {

		//Flush buffer
		twi_write_to(42, get_buffer(), BUFFER_LENGTH + BUFFER_PROLOG, TWI_BLOCK, TWI_STOP);
		
		_delay_ms(1000);
	}
}
