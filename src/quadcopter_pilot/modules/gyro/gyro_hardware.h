#include <avr/io.h>
/*
 * Gyroscope hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void init_gyro_hardware();

/* Returns the scaled gyroscope data for X axis */
uint16_t get_gyro_x();

/* Returns the scaled gyroscope data for Y axis */
uint16_t get_gyro_y();

/* Returns the scaled gyroscope data for Z axis */
uint16_t get_gyro_z();


