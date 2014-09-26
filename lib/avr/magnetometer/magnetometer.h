#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <math.h>

#include "lib/twi/twi.h"

/*
 * Initializes the magnetometer and performs a self calibration
 */
void magnetometer_init();

/*
 * Read the raw magnetometer data
 */
void magnetometer_read_raw(int16_t* data);

/*
 * Read the scaled magnetometer data, after applying gain factors
 */
void magnetometer_read_scaled(double* data);

/*
 * Read the compass heading, with 0 pointing straight north, positive values
 * pointing east, and negative values pointing west.  Value is in radians.
 */
double magnetometer_read_heading();

#endif