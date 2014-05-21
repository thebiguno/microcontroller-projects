#ifndef HEX_SERVO
#define HEX_SERVO

#include <avr/io.h>

#include "stubby.h"
#include "leg.h"
#include "lib/pwm/pwm.h"

//We assume a neutral offset of 1500, with even 
// amounts on either side.  We then assume that
// the servo has a linear travel between one end 
// and the other.
#define MIN_PHASE	700
#define NEUTRAL	1500
#define MAX_PHASE	2300

//Maximum angle of travel for the servo, in radians.
// Therefore, the maximum travel in each direction
// from neutral is half of this number.
#define SERVO_TRAVEL  3.1415926

void servo_init();

void servo_set_angle(uint8_t leg, uint8_t joint, double angle);

inline void servo_apply_batch(){
	pwm_apply_batch();
}

#endif
