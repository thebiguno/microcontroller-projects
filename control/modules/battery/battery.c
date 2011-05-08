#include "battery.h"
#include "../../../lib/analog/analog.h"

void battery_init(){
	uint8_t pins[1];
	pins[0] = 7;
	analog_init(pins, 1, ANALOG_AVCC);
}

double battery_level(){
	uint16_t raw = analog_read_p(0);
	double scaled = ((double) raw - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN);
	if (scaled < 0.0) scaled = 0.0;
	if (scaled > 1.0) scaled = 1.0;
	return scaled;
}
