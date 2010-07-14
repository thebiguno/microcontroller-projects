#include <avr/io.h>
/*
 * Gyroscope hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void init_gyro_hardware();

/* Returns the gyroscope data for X axis in rad/s */
double get_gyro_x();

/* Returns the gyroscope data for Y axis in rad/s */
double get_gyro_y();

/* Returns the gyroscope data for Z axis in rad/s */
double get_gyro_z();


