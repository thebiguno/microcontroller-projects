#ifndef GAIT_H
#define GAIT_H

#include <avr/io.h>
#include "Leg.h"
#include "Point.h"

using namespace digitalcave;

/*
 * Returns the offset for the given leg at the given step index
 */
Point gait_step(uint8_t leg_index, uint8_t step_index);

/*
 * Reteurns the number of steps in this gait
 */
uint8_t gait_step_count();

#endif