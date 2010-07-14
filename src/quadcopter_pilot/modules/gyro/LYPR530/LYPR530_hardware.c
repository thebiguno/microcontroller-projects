/*
 * LYPR530 implementation 
 */
#include "../gyro_hardware.h"
#include <math.h>

#define M 0.006141921121388
#define B M_PI * -1

// scale gyro output (0 .. 1023) to (-π .. π)
float scale(int8_t x) {
    // TODO this may need to be a non-linear scale dependending on the measured output of the gyro
    // If AREF = 3.3V, then A/D is 931 at 3V and 465 = 1.5V
    // y = mx + b 
    // m = (y2 - y1) / (x2 - x1) = (π - -π) / (1023 - 0) = 0.006141921121388
    // b = y1 - m * x1 = -π - 0.006141921121388 * 0 = -π
    return M * x + B;
}

void init_gyro_hardware(){

}

/* Returns the scaled gyroscope data for X axis */
float get_gyro_x(){
	//TODO
    return scale(0x7F);
}

/* Returns the scaled gyroscope data for Y axis */
float get_gyro_y(){
	//TODO
    return scale(0x7F);
}

/* Returns the scaled gyroscope data for Z axis */
float get_gyro_z(){
    // TODO
    return scale(0x7F);
}

