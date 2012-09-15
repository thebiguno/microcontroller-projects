#include "battery.h"
#include "../../../lib/analog/analog.h"

void battery_init(){
	uint8_t pins[1];
	pins[0] = 7;
	analog_init(pins, 1, ANALOG_AVCC);
}

double battery_voltage(){
	uint16_t raw = analog_read_p(0);
	
	//These values were obtained theoretically based on voltage divider math and AREF; 
	// the /2 comes from the voltage divider, and the /3 comes from the AVCC = 3v.
	// 680 = 4v		(4/2 / 3 * 1024)
	// 850 = 5v		(5/2 / 3 * 1024) 
	//I then performed interpolation to determine that the slope of the graph
	// is rise / run = 1v / 170 = 0.0059; the offset is 0.0.
	double voltage = 0.0059 * raw;
	return voltage;
}
