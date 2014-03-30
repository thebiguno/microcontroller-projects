#ifndef HEX_WAVE
#define HEX_WAVE

#include <avr/io.h>

#include "servo.h"
#include "leg.h"

#define WAVE_TIBIA_RAISED	600
#define WAVE_COXA_STEP		100

void wave_step();

void wave_gait_init();

#endif