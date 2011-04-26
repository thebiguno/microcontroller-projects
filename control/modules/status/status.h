#ifndef STATUS_H
#define STATUS_H

#include <stdlib.h>
#include <avr/io.h>
#include "../../main.h"

/*
 * Status hardware must implement these functions
 */
 
/*
 * Initialize hardware (will be called prior to main loop)
 */
void status_init();

/*
 * Sets the battery level (double from 0.0 to 1.0 inclusive)
 */
void status_set_battery_level(double value);

/*
 * Sets the telemetry values
 */
void status_set_telemetry();

#endif
