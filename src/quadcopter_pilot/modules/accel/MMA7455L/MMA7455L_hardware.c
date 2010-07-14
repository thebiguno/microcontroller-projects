#include "../accel_hardware.h"
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

void init_accel_hardware(){
	//TODO
}

/* Returns the scaled accelerometer data for X axis */
double get_accel_x(){
	//TODO
    return scale (0);
}

/* Returns the scaled accelerometer data for Y axis */
double get_accel_y(){
	//TODO
    return scale (0);
}

/* Returns the scaled accelerometer data for Z axis */
double get_accel_z(){
	//TODO
    return scale (0);
}


