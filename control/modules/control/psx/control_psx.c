/*
 * PSX control hardware implementation
 * Written by Wyatt Olson
 *
 *    ps1/ps2
 * 1. brown			dta
 * 2. orange		cmd
 * 3. violet/grey	vib (7.6v)
 * 4. black			gnd
 * 5. red			vcc (3.3v)
 * 6. yellow		att
 * 7. blue			clk
 * 8. black/white	n/c
 * 9. green			ack
 */

#include "../control.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/timer/timer.h"

//Pitch scale to radians
#define MAX_PITCH M_PI / 3				//Max pitch is PI/3 (60 degrees)
#define M_PITCH (2 * MAX_PITCH) / 256	//1 unit change in raw pitch is 0.0081... radians change
#define B_PITCH MAX_PITCH * -1			//Zero value

//Roll scale to radians
#define MAX_ROLL M_PI / 3				//Max roll is PI/3 radians (60 degrees)
#define M_ROLL (2 * MAX_ROLL) / 256		//1 unit change in raw roll is 0.0081... radians change
#define B_ROLL MAX_ROLL * -1			//Zero value

//Yaw rate scale to radians
#define R_YAW M_PI / 24.0				//7.5 degrees / second / input

//Throttle scale to a rate of change per second
#define MAX_THROTTLE 0.1				//Max throttle rate of change is 10% / second
#define M_THROTTLE (2 * MAX_THROTTLE) / 256
#define B_THROTTLE MAX_THROTTLE * -1

void control_init(){
	psx_init(&PORT_PSX_DATA, PIN_PSX_DATA, //Data (Brown)
			&PORT_PSX_CLOCK, PIN_PSX_CLOCK, //Clock (Blue)
			&PORT_PSX_COMMAND, PIN_PSX_COMMAND, //Command (Orange)
			&PORT_PSX_ATTENTION, PIN_PSX_ATTENTION); //Attention (Yellow)
}

static uint16_t button_state = 0;
static uint16_t button_changed = 0;

static double throttle = 0.0;

void control_update() {
	psx_read_gamepad();
	
	uint16_t read = psx_buttons();
	button_changed = button_state ^ read;
	button_state = read;
}

control_t control_read_analog() {
	// this code assumes "mode 2" which is the standard in North America
	// left stick = throttle
	// right stick = pitch / roll
	// however since the ps2 stick is spring loaded the throttle stick is a relative control rather than absolute
	// and the yaw controls are on the top buttons rather than on the throttle stick
	control_t result;
	
	result.pitch = 0;
	result.roll = 0;
	result.yaw = 0;
	result.throttle = 0;

	// Pitch and Roll are Absolute Linear Controls
	// Perform scaling into radians.  
	// Currently this is linear, we may want to change it to 
	uint8_t p = psx_stick(PSS_RY);
	if (p < 126 || p > 130) {
		result.pitch = M_PITCH * psx_stick(PSS_RY) + B_PITCH;
	}
	uint8_t r = psx_stick(PSS_RX);
	if (r < 126 || p > 130) {
		result.roll = M_ROLL * psx_stick(PSS_RX) + B_ROLL;
	}
	
	// The four Yaw buttons are Momentary Controls
	result.yaw = 0.0;
	uint16_t l1 = button_state & PSB_L1;
	uint16_t l2 = button_state & PSB_L2;
	uint16_t r1 = button_state & PSB_R1;
	uint16_t r2 = button_state & PSB_R2;

	//TODO these numbers are arbitrary, change them to actually make sense
	if (!r1 && !r2){
		if (l1 && l2) result.yaw = R_YAW * -6.0;  // -45 degrees/second
		else if (l2) result.yaw = R_YAW * -3.0;   // -22.5 degrees/second
		else if (l1) result.yaw = R_YAW * -1.0;   // -7.5 degrees/second
	}
	else if (!l1 && !l2){
		if (r1 && r2) result.yaw = R_YAW * 6.0;   // 45 degrees/second
		else if (r2) result.yaw = R_YAW * 3.0;    // 22.5 degrees/second
		else if (r1) result.yaw = R_YAW * 1.0;    // 7.5 degrees/second
	}
	
	// Throttle is a Relative Linear Control
	// Ignore the points around the centre of the stick
	// Scale into +/- 10% change / second range
	// Multiply by -1 to invert; up is more throttle, down is reverse throttle
	// Don't let the throttle exceed 80%; this leaves some overhead to pitch and roll and "full" throttle
	uint8_t t = psx_stick(PSS_LY);
	if (t < 126 || t > 130) {
		double delta = (M_THROTTLE * t + B_THROTTLE) * -1 * (dt * 0.001);
		throttle += delta;
		if (throttle > .8) throttle = .8;
		if (throttle < 0) throttle = 0;
	}
	result.throttle = throttle;
	return result;
}

uint16_t control_button_state(){
	uint16_t state = 0;

	if (button_state & PSB_START) state |= POWER;
	if (button_state & PSB_L3) state |= RESET_ATTITUDE;
	if (button_state & PSB_R3) state |= CALIBRATE;
	if (button_state & PSB_TRIANGLE) state |= MODE_SPORT;
	if (button_state & PSB_CROSS) state |= MODE_STABLE;

	return state;
}
uint16_t control_button_state_changed() {
	uint16_t changed = 0;

	if (button_changed & PSB_START) changed |= POWER;
	if (button_changed & PSB_L3) changed |= RESET_ATTITUDE;
	if (button_changed & PSB_R3) changed |= CALIBRATE;
	if (button_changed & PSB_TRIANGLE) changed |= MODE_SPORT;
	if (button_changed & PSB_CROSS) changed |= MODE_STABLE;

	return changed;
}

