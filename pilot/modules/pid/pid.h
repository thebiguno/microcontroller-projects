#ifndef PID_H
#define PID_H

#include "../../main.h"

void pid_init();

/*
 * Compute the correction to make based on the requested angle and the measured angle.
 * SP - the setpoint (from the controller) in radians
 * PV - the process variable (actual measured angle) in radians
 * DT - delta T in millis
 * MV - the manipulated variable (the correction) in radians
 */
vector_t pid_mv(vector_t sp, vector_t pv, double dt);
void pid_reset();

void pid_send_tuning();
void pid_receive_tuning(uint8_t *buf);

void pid_read_tuning();
void pid_write_tuning();

#endif
