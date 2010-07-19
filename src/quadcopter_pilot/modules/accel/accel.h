#ifndef ACCEL_H
#define ACCEL_H

#include <avr/io.h>
#include "main.h"

/*
 * Accelerometer hardware interface.  To be implemented on a per-hardware basis.
 */

/* Initializes the hardware */
void accel_init();

/* Returns the accelerometer data in m/s/s */
vector_t accel_get();

#endif
