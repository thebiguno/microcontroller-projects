#include "../accel.h"

/*
 * MMA7455L implementation 
 */

#define M 0.062992125984252
#define B 0

// scale accel output (-127 .. 127) to (-8 .. 8)
double scale(int16_t x) {
    // y = mx + b 
    // m = (y2 - y1) / (x2 - x1) = (8 - -8) / (127 - -127) = 0.062992125984252
    // b = y1 - m * x1 = -8 - 0.062992125984252 * -127 = 0
    return M * x + B;
}

void accel_init(){
	//TODO
}

vector_t accel_get() {
    vector_t result;
	//TODO
    result.x = scale(0);
    result.y = scale(0);
    result.z = scale(0);
    return result;
}