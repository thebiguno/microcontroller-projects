#ifndef MOTOR_H
#define MOTOR_H

#include "../../main.h"
#include "../util/convert.h"

void motor_init();

void motor_send_tuning();
void motor_receive_tuning(uint8_t *buf);

void motor_read_tuning();
void motor_write_tuning();

/*
 * Given a throttle percentage (0 .. 1) and vector of percentage adjustments for each axis (-1 .. 1), calculate the motor power percentage (0..1)
 */
void motor_percent(double throttle, vector_t mv, double result[]);

#endif
