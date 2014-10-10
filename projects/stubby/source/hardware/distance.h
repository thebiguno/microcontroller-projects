#ifndef DISTANCE_SENSOR_H
#define DISTANCE_SENSOR_H

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

#define TRIGGER_PORT		PORTA
#define TRIGGER_PIN			PORTA0
#define ECHO_PORT			PINA
#define ECHO_PIN			PINA1

/*
 * Takes a distance reading, and adds to the running average.
 */
void distance_measure();

/*
 * Read the filtered distance in mm
 */
uint16_t distance_read();

#endif