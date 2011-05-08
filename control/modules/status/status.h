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
 * Sets the battery level for the pilot (double from 0.0 to 1.0 inclusive)
 */
void status_set_pilot_battery_level(double value);

/*
 * Sets the battery level for the control (double from 0.0 to 1.0 inclusive)
 */
void status_set_control_battery_level(double value);


/*
 * Sets the throttle (double from 0.0 to 1.0 inclusive).  If 
 * armed, then show throttle; otherwise show ---
 */
void status_set_throttle(double value, uint8_t armed);

/*
 * Sets the telemetry values on the display (double values, in degrees; level is 0)
 */
void status_set_telemetry(double pitch, double roll);

/*
 * Sets the current armed time, in milliseconds.
 */
void status_set_armed_time(uint32_t millis);

/*
 * Sets rx / tx icons depending on communication status
 */
void status_set_comm_state(uint8_t tx, uint8_t rx);

#endif
