#include <dcutil/delay.h>

#define __DELAY_BACKWARD_COMPATIBLE__
#include "util/delay.h"

void delay_ms(uint32_t delay){
	_delay_ms(delay);
}

void delay_us(uint32_t delay){
	_delay_us(delay);
}
