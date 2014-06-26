#ifndef GAIT_H
#define GAIT_H

#include <avr/io.h>
#include "Leg.h"
#include "Point.h"

using namespace digitalcave;

/*
 * Returns the offset for the given leg at the given step index, scaled by the given velocity
 */
Point gait_step(uint8_t leg_index, uint8_t step_index, double linear_velocity, double linear_angle, double rotational_velocity);

/*
 * Reteurns the number of steps in this gait
 */
uint8_t gait_step_count();

#endif