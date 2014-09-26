#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>

#include "lib/twi/twi.h"

#define MAGNETOMETER_ADDRESS 0x1E

void magnetometer_init();
double magnetometer_read_heading();

#endif
