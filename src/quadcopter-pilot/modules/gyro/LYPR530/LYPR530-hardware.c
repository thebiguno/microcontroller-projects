/*
 * LYPR530 implementation 
 */
#include "../gyro-hardware.h"

// scale gyro output (-465 .. +465) to (0 .. 65535)
uint16_t scale(int8_t x) {
    // If AREF = 3.3V, then A/D is 931 at 3V and 465 = 1.5V 
    // y = mx + b 
    // m = (y2 - y1) / (x2 - x1) = (65535 - 0) / (465 - (-465)) = 70.467741935483871;
    // b = y1 - m * x1 = 0 - 70.467741935483871 * -465 = -32767
    float m = 70.467741935483871; 
    float b = -32767;
    return (uint16_t) m * x + b;
}

void init_gyro_hardware(){

}

/* Returns the scaled gyroscope data for X axis */
uint16_t get_gyro_x(){
	//TODO
    return scale(0x7F);
}

/* Returns the scaled gyroscope data for Y axis */
uint16_t get_gyro_y(){
	//TODO
    return scale(0x7F);
}

/* Returns the scaled gyroscope data for Z axis */
uint16_t get_gyro_z(){
    // TODO
    return scale(0x7F);
}

