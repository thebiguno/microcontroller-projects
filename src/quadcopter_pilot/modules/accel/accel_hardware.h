#include <avr/io.h>

/*
 * Accelerometer hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void init_accel_hardware();

/* Returns the scaled accelerometer data for X axis */
float get_accel_x();

/* Returns the scaled accelerometer data for Y axis */
float get_accel_y();

/* Returns the scaled accelerometer data for Z axis */
float get_accel_z();


