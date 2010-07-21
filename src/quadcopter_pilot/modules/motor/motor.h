#ifndef MOTOR_H
#define MOTOR_H

#include "../../main.h"


#define M 0.636619772367581
#define B 0.0

// y = mx + b 
// m = (y2 - y1) / (x2 - x1) = (-1 - 1) / (-1.570796326794897 - 1.570796326794897) = 0.636619772367581
// b = y1 - m * x1 = 1 - 0.636619772367581 * 1.570796326794897 = 0
#define RAD_TO_THROTTLE_ADJUSTMENT(x) M * (x) + B


/*
 * Given a throttle percentage (0 .. 1) and vector of percentage adjustments for each axis (-1 .. 1), calculate the motor power percentage (0..1)
 */
void motor_cmd(double *result, double throttle, vector_t adj);

#endif
