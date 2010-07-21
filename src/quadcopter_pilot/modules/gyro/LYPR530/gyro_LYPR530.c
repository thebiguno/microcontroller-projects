/*
 * LYPR530 implementation 
 */
#include "../gyro.h"

#define M 0.006141921121388
#define B M_PI * -1

// TODO this may need to be a non-linear scale dependending on the measured output of the gyro
// If AREF = 3.3V, then A/D is 931 at 3V and 465 = 1.5V
// y = mx + b 
// m = (y2 - y1) / (x2 - x1) = (π - -π) / (1023 - 0) = 0.006141921121388
// b = y1 - m * x1 = -π - 0.006141921121388 * 0 = -π
#define SCALE_TO_RAD(x) (M * (x) + B)

void gyro_init(){
    ;
}

vector_t gyro_get() {
    vector_t result;
	//TODO
    result.x = SCALE_TO_RAD(0x7F);
    result.y = SCALE_TO_RAD(0x7F);
    result.z = SCALE_TO_RAD(0x7F);
    return result;
}
