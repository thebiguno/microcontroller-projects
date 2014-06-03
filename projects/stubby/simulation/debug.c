#include "debug.h"

void debug_int(uint8_t id, int32_t value){
	printf(" Id: %d, Value: %d\n", id, value);
}
void debug_double(uint8_t id, double value){
	printf(" Id: %d, Value: %f\n", id, value);
}

void pwm_set_phase_batch(uint8_t index, uint16_t phase){
	printf("Index: %d, Phase: %d\n\n", index, phase);
}
