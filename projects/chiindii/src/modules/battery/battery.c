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

double battery_voltage(){
	uint16_t raw = analog_read_p(2); //Index 2 is the ADC Battery index.

	//These magic values were obtained by experimentation on Wyatt's craft:
	// given three ADC readings + Multimeter references, I determined that:
	// 960 = 11.21v
	// 969 = 11.30v
	// 1015 = 11.83v
	//I then performed interpolation to determine that the slope of the graph
	// is rise / run = 0.62v / 55 = 0.01127; the offset is 0.38818.  Therefore,
	// for my craft the equation is y = voltage = mx + b = 0.01127 * ADC + 0.38818.
	//Repeat with actual experimental values for extra accuracy / other craft.
	double voltage = 0.01127 * raw + 0.38818;
	return voltage;
}
