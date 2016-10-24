#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <math.h>
#include <twi/twi.h>

#include "../Stubby.h"
#include "status.h"
#include "../util/convert.h"


/*
 * Initializes the magnetometer and performs a self calibration
 */
void magnetometer_init();

/*
 * Get the currently set offsets (x & y).
 */
void magnetometer_get_offsets(int16_t *x, int16_t *y);

/*
 * Set the offsets (x & y).  This is used to center the readings around zero.  These
 * values will depend on the location and surroundings of the magnetometer, and should
 * be obtained from an in-situ calibration.
 */
void magnetometer_set_offsets(int16_t x, int16_t y);

/*
 * Read the raw values into x and y
 */
void magnetometer_get_raw(int16_t *x, int16_t *y);

/*
 * Read the compass heading, with 0 pointing straight north, positive values
 * pointing east, and negative values pointing west.  Value is in radians.
 */
double magnetometer_read_heading();

#endif