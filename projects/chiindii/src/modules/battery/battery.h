#ifndef BATTERY_H
#define BATTERY_H


//When calculating these, the /4.93 comes from the voltage divider (R1=2kΩ, R2=510Ω), 
// and the /2.56 comes from the AVCC = 2.56v.
#define BATTERY_MIN 810		//About 10.0v (10.0/4.93 / 2.56 * 1024).
#define BATTERY_MAX 940	//About 11.6v (11.6/4.93 / 2.56 * 1024).

#include "../../main.h"

/*
 * Initializes the module, setting up the ADC.
 */
void battery_init();

/*
 * Returns the current battery level, in volts.  This may need to be calibrated on a 
 * per-craft basis.
 */
double battery_voltage();

#endif
