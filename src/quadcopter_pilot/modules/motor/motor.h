#ifndef MOTOR_H
#define MOTOR_H

#include "../../main.h"

#define MOTOR_A = 0
#define MOTOR_B = 1
#define MOTOR_C = 2
#define MOTOR_D = 3

/*
 * Given a throttle percentage (0 .. 1) and vector of percentage adjustments for each axis (-1 .. 1), calculate the motor power percentage (0..1)
 */
void motor_cmd(double *result, double throttle, vector_t adj);

#endif
