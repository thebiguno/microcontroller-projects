#include "shift.h"

void shiftOut(volatile uint8_t *data_port, uint8_t data_pin, volatile uint8_t *clock_port, uint8_t clock_pin, volatile uint8_t *latch_port, uint8_t latch_pin, uint8_t data){
	*latch_port &= ~(1 << latch_pin);	

	for (int i = 0; i < 8; i++){
		//Clear the pin first...
		*data_port &= ~(1 << data_pin);
		//... then set the bit (if appropriate).  We could probably
		// do this in one step, but this is more clear, plus speed is 
		// not critical here.
		*data_port |= (((data >> (7 - i)) & 0x1) << data_pin);
		
		//Pulse clock to shift in
		*clock_port &= ~(1 << clock_pin);
		*clock_port |= (1 << clock_pin);
	}

	*latch_port |= (1 << latch_pin);	
}
