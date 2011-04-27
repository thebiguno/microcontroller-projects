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
 * Sets the telemetry values on the display (double values, in degrees; level is 0)
 */
void status_set_telemetry(double pitch, double roll);

/*
 * Sets the motor values on the display (double value from 0.0 to 1.0)
 */
void status_set_motors(double front, double right, double back, double left);

/*
 * Sets armed if value != 0; unarmed if value == 0.
 */
void status_set_armed(uint8_t armed);

#endif
