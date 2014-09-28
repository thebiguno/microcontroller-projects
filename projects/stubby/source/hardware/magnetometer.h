#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <avr/interrupt.h>
#include <math.h>
#include <util/delay.h>

#include "lib/twi/twi.h"

#include "status.h"

#include "Stubby.h"

/*
 * Initializes the magnetometer and performs a self calibration
 */
void magnetometer_init();

/*
 * Set the offsets (x & y).  This is used to center the readings around zero.  These
 * values will depend on the location and surroundings of the magnetometer, and should
 * be obtained from an in-situ calibration.
 */
void magnetometer_set_offsets(int16_t x, int16_t y);

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