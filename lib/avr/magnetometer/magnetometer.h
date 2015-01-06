#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <math.h>

#include "lib/twi/twi.h"

/*
 * Initializes the magnetometer and performs a self calibration
 */
void magnetometer_init();

/*
 * Set the offsets (x, y, and z).  This is used to center the readings around zero.  These
 * values will depend on the location and surroundings of the magnetometer, and should
 * be obtained from an in-situ calibration.
 */
void magnetometer_set_offsets(int16_t x, int16_t y, int16_t z);

/*
 * Read the raw magnetometer data
 */
void magnetometer_read_raw(int16_t* data);

/*
 * Read the compass heading, with 0 pointing straight north, positive values
 * pointing east, and negative values pointing west.  Value is in radians.
 */
double magnetometer_read_heading();

#endif