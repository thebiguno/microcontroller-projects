#ifndef HEX_RIPPLE
#define HEX_RIPPLE

#define DELAY 	32
#define COXA_STEP	0.3
#define TIBIA_STEP	1

#include <avr/io.h>

#include "leg.h"

void ripple_step();

void ripple_gait_init();

#endif