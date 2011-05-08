#include "battery.h"
#include "../../../lib/analog/analog.h"

void battery_init(){
	//We do the ADC init in gyro; if gyro doesnt use ADC anymore, 
	// uncomment this.  Yeah, it's not completely modular, but that's
	// life on a micro controller.  :-(
	//uint8_t pins[1];
	//pins[0] = ADC_BATTERY;
	//analog_init(pins, 1, ANALOG_AVCC);
}

double battery_level(){
	uint16_t raw = analog_read_p(2); //Index 2 is the ADC Battery index.
	double scaled = ((double) raw - BATTERY_MIN) / (BATTERY_MAX - BATTERY_MIN);
	if (scaled < 0.0) scaled = 0.0;
	if (scaled > 1.0) scaled = 1.0;
	return scaled;
}
