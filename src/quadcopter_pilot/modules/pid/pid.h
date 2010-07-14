#ifndef PID_H
#define PID_H

#define PID_PITCH = 0
#define PID_ROLL = 1
#define PID_YAW = 2

void init_pid();

/*
 * Given a vector of setpoints, a vector of process variables, return a vector of manipulated variables
 */
 vector_t mv(vector_t sp, vector_t pv) {

#endif
