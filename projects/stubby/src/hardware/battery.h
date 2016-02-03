#ifndef BATTERY_H
#define BATTERY_H

#include <avr/io.h>

#include "status.h"

#include "../lib/pwm/pwm.h"

#if PCB_REVISION == 1
	//The minimum ADC value which indicates a full battery
	#define BATTERY_LEVEL_FULL		180
	//The maximum ADC value which indicates an empty battery
	#define BATTERY_LEVEL_EMPTY		150
#elif PCB_REVISION == 2
	//The minimum ADC value which indicates a full battery
	#define BATTERY_LEVEL_FULL		128
	//The maximum ADC value which indicates an empty battery
	#define BATTERY_LEVEL_EMPTY		100
#else
#warning Non-standard PCB revision specified.  Please calibrate battery.
#endif

//Defines a linear progression for valid battery values.
#define BATTERY_STATUS_MULTIPLIER		255 / (BATTERY_LEVEL_FULL - BATTERY_LEVEL_EMPTY)

void battery_init();
void battery_enable_status();
void battery_disable_status();
void battery_set_level();

#endif
