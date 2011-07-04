#ifndef PID_H
#define PID_H

#include "../../main.h"

void pid_init();

/*
 * Given a vector of setpoints, a vector of process variables, return a vector 
 * of manipulated variables
 */
vector_t pid_mv(vector_t sp, vector_t pv, double dt);
void pid_reset();

void pid_send_tuning();
void pid_receive_tuning(uint8_t *buf);

void pid_read_tuning();
void pid_write_tuning();

#endif
