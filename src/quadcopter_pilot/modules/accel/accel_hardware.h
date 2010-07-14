#include <avr/io.h>

/*
 * Accelerometer hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void init_accel_hardware();

/* Returns the accelerometer data for X axis in m/s/s */
double get_accel_x();

/* Returns the accelerometer data for Y axis in m/s/s */
double get_accel_y();

/* Returns the accelerometer data for Z axis in m/s/s */
double get_accel_z();


