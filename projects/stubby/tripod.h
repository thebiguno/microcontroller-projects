#ifndef HEX_TRIPOD
#define HEX_TRIPOD

#include <avr/io.h>

#include "servo.h"
#include "leg.h"

#define TRIPOD_TIBIA_RAISED	800
#define TRIPOD_COXA_STEP	400

void ripple_step();

void ripple_gait_init();

#endif