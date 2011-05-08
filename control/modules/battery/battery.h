#ifndef BATTERY_H
#define BATTERY_H

//When calculating these, the /2 comes from the voltage divider, and the /3 comes
// from the AVCC = 3v.
#define BATTERY_MIN 680		//About 4v (4/2 / 3 * 1024)
#define BATTERY_MAX 850		//About 5v (5/2 / 3 * 1024) 

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
