#ifndef ACCEL_H
#define ACCEL_H

#include "../../main.h"

/*
 * Accelerometer hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void accel_init();

/* Start calibration routines.  The aircraft should be level when this is called. */
void accel_calibrate();

/* Returns the accelerometer data in m/s/s */
vector_t accel_get();

#endif
