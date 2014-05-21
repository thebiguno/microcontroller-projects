#ifndef GAIT
#define GAIT

#define TIBIA_STEP	(TIBIA_RAISED - TIBIA_LOWERED)

#include <avr/io.h>

#include "comm.h"
#include "leg.h"

void tripod_gait_step();

void gait_init();

#endif