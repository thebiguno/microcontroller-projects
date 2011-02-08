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

//Pitch limits
#define MAX_PITCH M_PI / 3				//Max pitch is ¹/3 (60 degrees)
#define M_PITCH (2 * MAX_PITCH) / 256	//1 unit change in raw pitch is 0.0081... radians change
#define B_PITCH MAX_PITCH * -1			//Zero value

//Roll limits
#define MAX_ROLL M_PI / 3				//Max roll is ¹/3 (60 degrees)
#define M_ROLL (2 * MAX_ROLL) / 256		//1 unit change in raw roll is 0.0081... radians change
#define B_ROLL MAX_ROLL * -1			//Zero value

//Yaw rate
#define R_YAW M_PI / 24					//7.5 degrees / second / input

//Throttle limits
#define MAX_THROTTLE 1.0				//Max throttle is 1
#define M_THROTTLE 2 * MAX_THROTTLE / 256
#define B_THROTTLE MAX_THROTTLE * -1

#define BOUNCE_MAX_CHECKS 10

void control_init(){
	psx_init(&PORT_PSX_DATA, PIN_PSX_DATA, //Data (Brown)
			&PORT_PSX_CLOCK, PIN_PSX_CLOCK, //Clock (Blue)
			&PORT_PSX_COMMAND, PIN_PSX_COMMAND, //Command (Orange)
			&PORT_PSX_ATTENTION, PIN_PSX_ATTENTION); //Attention (Yellow)
}

uint16_t button_state; // debounced state of the buttons
uint16_t bounce_state[BOUNCE_MAX_CHECKS]; // array that maintains bounce status
uint8_t bounce_index; // index into bounce_state
uint16_t state_changed;

uint8_t osc; // state of the one-shot controls

void control_update() {
	psx_read_gamepad();
	
	// TODO verify if debouncing is really required or if the psx is debouncing for us
	// debounce all 16 buttons at once
	// based on code from http://www.ganssle.com/debouncing-pt2.htm
	// last 10 readings must all be 1 for the state to be 1, otherwise it's 0
	uint8_t i;
	uint16_t j;
	bounce_state[bounce_index] = psx_buttons();
	++bounce_index;
	j = 0xffff;
	for (i = 0; i < BOUNCE_MAX_CHECKS; i++) j = j & bounce_state[i];
	state_changed = button_state ^ j;
	button_state = j;
	if (bounce_index >= BOUNCE_MAX_CHECKS) bounce_index = 0;
}

static inline uint8_t _control_button_state(uint16_t button) {
	return ((~button_state & button) > 0);
}
static inline uint8_t _control_button_state_changed(uint16_t button) {
	return ((~state_changed & button) > 0);
}

control_t control_read_analog(){
	control_t result;

	// Pitch and Roll are Absolute Linear Controls
	// Perform scaling into radians.  
	// Currently this is linear, we may want to change it to 
	result.pitch = M_PITCH * psx_stick(PSS_LY) + B_PITCH;
	result.roll = M_ROLL * psx_stick(PSS_LX) + B_ROLL;
	
	// The four Yaw buttons are Momentary Controls
	result.yaw = 0.0;
	uint8_t l1 = _control_button_state(PSB_L1);
	uint8_t l2 = _control_button_state(PSB_L2);
	uint8_t r1 = _control_button_state(PSB_R1);
	uint8_t r2 = _control_button_state(PSB_R2);

	//TODO these numbers are arbitrary, change them to actually make sense
	if (!r1 && !r2){
		if (l1 && l2) result.yaw = R_YAW * -6;  // -45 degrees
		else if (l2) result.yaw = R_YAW * -3;   // -22.5 degrees
		else if (l1) result.yaw = R_YAW * -1;   // -7.5 degrees
	}
	else if (!l1 && !l2){
		if (r1 && r2) result.yaw = R_YAW * 6;   // 45 degrees
		else if (r2) result.yaw = R_YAW * 3;    // 22.5 degrees
		else if (r1) result.yaw = R_YAW * 1;    // 7.5 degrees
	}
	
	// Throttle is a Relative Linear Control
	// Scale into -1 .. 1 range.  We multiply the entire thing by -1 to invert;
	// up is more throttle, down is reverse throttle
	result.throttle =+ (M_THROTTLE * psx_stick(PSS_RY) + B_THROTTLE) * -1;
	return result;
}

uint16_t control_button_state(){
	uint16_t buttons = 0x0;

	// power is a relative on/off control
	if (_control_button_state(PSB_START)) buttons |= POWER;
	
	// reset and calibrate are one-shot controls
	if (_control_button_state(PSB_L3)) buttons |= RESET_ATTITUDE;
	if (_control_button_state(PSB_R3)) buttons |= CALIBRATE;
	
	// mode buttons are one-shot controls
	if (_control_button_state(PSB_SQUARE)) buttons |= MODE_AEROBATIC;
	if (_control_button_state(PSB_CIRCLE)) buttons |= MODE_SPORT;
	if (_control_button_state(PSB_CROSS)) buttons |= MODE_STABLE;
	if (_control_button_state(PSB_TRIANGLE)) buttons |= MODE_HOVER;

	return buttons;
}
uint16_t control_button_state_changed() {
	uint16_t buttons = 0x0;

	if (_control_button_state_changed(PSB_START)) buttons |= POWER;
	if (_control_button_state_changed(PSB_L3)) buttons |= RESET_ATTITUDE;
	if (_control_button_state_changed(PSB_R3)) buttons |= CALIBRATE;
	
	// mode buttons not included because it is safe to read stale values
	
	return buttons;
}

