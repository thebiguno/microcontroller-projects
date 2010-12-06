#ifndef PID_H
#define PID_H

#include "../../main.h"

void pid_init();


void pid_set_params(vector_t *kp, vector_t *ki, vector_t *kd);
void pid_get_params(vector_t *kp, vector_t *ki, vector_t *kd);

/*
 * Given a vector of setpoints, a vector of process variables, return a vector 
 * of manipulated variables
 */
vector_t pid_mv(vector_t sp, vector_t pv);

#endif
