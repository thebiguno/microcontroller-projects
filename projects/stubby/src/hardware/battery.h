#ifndef BATTERY_H
#define BATTERY_H

#include <avr/io.h>

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

void battery_init();
uint8_t battery_get_percent();

#endif
