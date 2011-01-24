#ifndef MOTOR_H
#define MOTOR_H

#include "../../main.h"

/*
 * Given a throttle percentage (0 .. 1) and vector of percentage adjustments for each axis (-1 .. 1), calculate the motor power percentage (0..1)
 */
void motor_percent(double throttle, vector_t mv, double result[]);

#endif
