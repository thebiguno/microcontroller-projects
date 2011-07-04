#ifndef BATTERY_H
#define BATTERY_H


//When calculating these, the /3.83 comes from the voltage divider (R1=510Ω, R2=180Ω), 
// and the /3.3 comes from the AVCC = 3.3v.
#define BATTERY_MIN 780		//About 9.6v (9.6/3.83 / 3.3 * 1024)
#define BATTERY_MAX 1020	//About 12.6v (12.6/3.83 / 3.3 * 1024) 

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
