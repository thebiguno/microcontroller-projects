#ifndef GYRO_H
#define GYRO_H

#include "../../main.h"

/*
 * Gyroscope hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void gyro_init();

/* Start calibration routines.  The aircraft should be level when this is called. */
void gyro_calibrate();

/* Returns the gyroscope data in rad/s */
vector_t gyro_get();

#endif
