#ifndef HEX_RIPPLE
#define HEX_RIPPLE

#define MIN_DELAY 	40
#define MAX_DELAY	100
#define COXA_STEP	0.3
#define TIBIA_STEP	1

#include <avr/io.h>

#include "leg.h"

void ripple_step();

void ripple_gait_init();

#endif