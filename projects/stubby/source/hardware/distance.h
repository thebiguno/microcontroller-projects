#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

/*
 * Initializes the distance sensor hardware
 */
void distance_init();

/*
 * Takes a distance reading, and adds to the running average.
 */
void distance_measure();

/*
 * Read the filtered distance in mm
 */
uint16_t distance_read();

#endif