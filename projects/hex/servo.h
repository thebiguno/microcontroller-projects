#ifndef HEX_SERVO
#define HEX_SERVO

#include <avr/io.h>

#include "leg.h"

#define MIN_PHASE	800
#define NEUTRAL	1500
#define MAX_PHASE	2200

void servo_init();

#endif

