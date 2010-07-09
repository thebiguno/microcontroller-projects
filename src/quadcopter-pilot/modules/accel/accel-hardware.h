#include <avr/io.h>

/*
 * Accelerometer hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void init_accel_hardware();

/* Returns the scaled accelerometer data for X axis */
uint16_t get_accel_x();

/* Returns the scaled accelerometer data for Y axis */
uint16_t get_accel_y();

/* Returns the scaled accelerometer data for Z axis */
uint16_t get_accel_z();


