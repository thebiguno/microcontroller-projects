#ifndef GAIT_H
#define GAIT_H

#ifndef DEBUG_SIMULATION
#include <avr/io.h>
#else
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#endif

#include "../types/Point.h"
#include "../Leg.h"

/*
 * Returns the offset for the given leg at the given step index, scaled by the given velocity
 */
Point gait_step(Leg leg, uint8_t step_index, double linear_velocity, double linear_angle, double rotational_velocity);

/*
 * Reteurns the number of steps in this gait
 */
uint8_t gait_step_count();

#endif
