/*
 * LYPR530 implementation 
 */
#include "../gyro.h"
#include <math.h>

#define M 0.006141921121388
#define B M_PI * -1

// scale gyro output (0 .. 1023) to (-π .. π)
double scale(int8_t x) {
    // TODO this may need to be a non-linear scale dependending on the measured output of the gyro
    // If AREF = 3.3V, then A/D is 931 at 3V and 465 = 1.5V
    // y = mx + b 
    // m = (y2 - y1) / (x2 - x1) = (π - -π) / (1023 - 0) = 0.006141921121388
    // b = y1 - m * x1 = -π - 0.006141921121388 * 0 = -π
    return M * x + B;
}

void init_gyro_hardware(){

}

vector_t get_gyro() {
    vector_t result;
	//TODO
    result.x = scale(0x7F);
    result.y = scale(0x7F);
    result.z = scale(0x7F)
    return result;
}
