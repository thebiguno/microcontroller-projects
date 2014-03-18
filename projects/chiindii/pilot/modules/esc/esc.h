#ifndef ESC_H
#define ESC_H

#include "../../main.h"

void esc_init();

/*
 * Perform the calibration routine.  Depending on the ESCs, this MAY
 * block forever.
 */
void esc_calibrate();

/*
 * Should the calibration routine be called?  Return 0 if false, non-0 
 * if true.  The implementation will probably check a jumper or some
 * other way to get human intervention.
 */
uint8_t esc_calibration_required();

/*
 * send the motor speed percentages (0 .. 1) to the speed controllers
 */
void esc_set(double speed[]);

#endif
