#ifndef GYRO_H
#define GYRO_H

#include "../main.h"
#include <avr/io.h>
/*
 * Gyroscope hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void init_gyro();

/* Returns the gyroscope data in rad/s */
vector_t get_gyro();

#endif
