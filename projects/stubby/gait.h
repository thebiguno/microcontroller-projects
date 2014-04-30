#ifndef GAIT
#define GAIT

#define COXA_STEP (COXA_FORWARD - COXA_REVERSE) / 5
#define TIBIA_STEP	(TIBIA_RAISED - TIBIA_LOWERED)

#include <avr/io.h>

#include "comm.h"
#include "leg.h"

void gait_step();

void gait_init();

#endif