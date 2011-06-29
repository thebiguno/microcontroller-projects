#ifndef BATTERY_H
#define BATTERY_H


//When calculating these, the /3.55 comes from the voltage divider, and the /3.3 comes
// from the AVCC = 3v.
#define BATTERY_MIN 839		//About 9.6v (9.6/3.55 / 3.3 * 1024)
#define BATTERY_MAX 1024	//About 12v (12/3.55 / 3.3 * 1024) 

#include "../../main.h"

/*
 * Initializes the module, setting up the ADC.
 */
void battery_init();

/*
 * Returns the current battery level.  0 is dead, 255 is full.  Adjust defines to
 * set what the max / min cutoff is.
 */
double battery_level();

#endif
