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
 * Initializes the persistent text / graphics for each mode
 */
void status_init_mode_flight();
void status_init_mode_calibrate();
void status_init_mode_version();
void status_init_mode_pid();
void status_init_mode_comp();
void status_init_mode_kalman();


/////// Flight mode functions
/*
 * Sets the battery level for the pilot (actual voltage level)
 */
void status_set_pilot_battery_level(double voltage);
/*
 * Sets the battery level for the control (actual voltage level)
 */
void status_set_control_battery_level(double voltage);
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
/* Sets the motor values on the display (double value from 0.0 to 1.0)
 */
void status_set_motors(double left, double front, double right, double back);


/////// Calibration mode functions
/* No calibration display needed */

/////// Version mode functions

void status_set_version_values(char* pilot_version, char* control_version);

/////// PID mode functions
/*
 * Sets PID values for pitch (x) and roll (y).  row / col specifies the selected value.
 */
void status_set_pid_values(uint8_t col, uint8_t row, vector_t p, vector_t i, vector_t d);

/////// Complementary mode functions
/*
 * Sets complementary tuning values.  row specifies the selected value.
 */
void status_set_comp_values(uint8_t row, vector_t k);

/////// Kalman mode functions
/*
 * Sets kalman tuning values.  row / col specifies the selected value.
 */
void status_set_kalman_values(uint8_t col, uint8_t row, vector_t kalman_qa, vector_t kalman_qg, vector_t kalman_ra);

/*
 * Thin wrapper to invert display on persist.
 */
void status_persist_values(uint8_t invert);


//Warning functions: these will be implemented by a high-attention module, probably
// something like a buzzer or strobe.  They are all error conditions.
void status_error_init();

void status_error_battery(uint8_t error);

#endif
