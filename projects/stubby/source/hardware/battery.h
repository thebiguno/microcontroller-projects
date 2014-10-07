#ifndef BATTERY_H
#define BATTERY_H

#include <avr/io.h>

#include "status.h"

#include "../lib/pwm/pwm.h"


//The minimum ADC value which indicates a full battery
#define BATTERY_LEVEL_FULL		180
//The maximum ADC value which indicates an empty battery
#define BATTERY_LEVEL_EMPTY		150

//The magic number 78 comes from X/20000 = color/255; X = color * 20000 / 255; X = color * 78.
#define PHASE_MULTIPLIER		(uint8_t) (20000/255)

//Defines a linear progression for valid battery values.
#define BATTERY_STATUS_MULTIPLIER		255 / (BATTERY_LEVEL_FULL - BATTERY_LEVEL_EMPTY) * PHASE_MULTIPLIER

void battery_init();
void battery_enable_status();
void battery_disable_status();
void battery_set_level();

#endif
