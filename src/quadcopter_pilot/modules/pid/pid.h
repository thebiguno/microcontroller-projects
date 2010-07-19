#ifndef PID_H
#define PID_H

#include "../../main.h"

void pid_init();

/*
 * Given a vector of setpoints, a vector of process variables, return a vector 
 * of manipulated variables
 */
vector_t pid_mv(vector_t sp, vector_t pv);

#endif
