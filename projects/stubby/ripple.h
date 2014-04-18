#ifndef HEX_RIPPLE
#define HEX_RIPPLE

#define DELAY 		80
#define COXA_STEP (COXA_FORWARD - COXA_REVERSE) / 5
#define TIBIA_STEP	(TIBIA_RAISED - TIBIA_LOWERED)

#include <avr/io.h>

#include "comm.h"
#include "leg.h"

void ripple_step();

void ripple_gait_init();

#endif