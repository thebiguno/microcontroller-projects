#ifndef DCUTIL_DELAY
#define DCUTIL_DELAY
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	
void delay_ms(uint32_t delay);

void delay_us(uint32_t delay);

#ifdef __cplusplus
}
#endif

#endif