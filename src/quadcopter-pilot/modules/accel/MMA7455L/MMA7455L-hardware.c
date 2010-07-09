#include "../accel-hardware.h"
/*
 * MMA7455L implementation 
 */


// scale accel output (0 .. 1023) to (0 .. 65535)
uint16_t scale(uint16_t x) {
    // y = mx + b 
    // m = (y2 - y1) / (x2 - x1) = (65535 - 0) / (1023 - 0) = 64.06158357771261;
    // b = y1 - m * x1 = 0 - 64.06158357771261 * 0 = 0
    float m = 64.06158357771261; 
    float b = 0;
    return (uint16_t) m * x + b;
}

void init_accel_hardware(){
	//TODO
}

/* Returns the scaled accelerometer data for X axis */
uint16_t get_accel_x(){
	//TODO
    return scale (0x1FF);
}

/* Returns the scaled accelerometer data for Y axis */
uint16_t get_accel_y(){
	//TODO
    return scale (0x1FF);
}

/* Returns the scaled accelerometer data for Z axis */
uint16_t get_accel_z(){
	//TODO
    return scale (0x1FF);
}


