#ifndef HEADING_H
#define HEADING_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <stdio.h>
#include <util/delay.h>

#include "../util/convert.h"
#include "../lib/magnetometer/magnetometer.h"

/*
 * Initialize the magnetometer hardware, start the timer for
 * low pass software filter, and initialize filtered
 * heading.
 */
void heading_init();

/*
 * Returns the current filtered heading
 */
double heading_read();


#endif