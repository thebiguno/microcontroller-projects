#ifndef HEX_RIPPLE
#define HEX_RIPPLE

#include <avr/io.h>

#include "servo.h"
#include "leg.h"

#define RIPPLE_TIBIA_RAISED	600
#define RIPPLE_COXA_STEP	100

void ripple_step();

void ripple_gait_init();

#endif