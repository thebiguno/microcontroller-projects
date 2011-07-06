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

#include <math.h>
#include "../control.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/timer/timer.h"

//Throttle scale to a rate of change per second
#define MAX_THROTTLE 0.25				//Max throttle rate of change is 25% / second
#define M_THROTTLE (2 * MAX_THROTTLE) / 128
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

	// Pitch and Roll are Absolute Linear Controls
	// Perform scaling into radians.  
	// The scaling follows a sin curve and is limited to about 10 degrees (pi/18 radians)
	uint8_t p = psx_stick(PSS_RY);
	result.pitch = (tan((p - 128) / (32 * M_PI))) / (5 * M_PI);
	uint8_t r = psx_stick(PSS_RX);
	result.roll = (tan((r - 128) / (32 * M_PI))) / (5 * M_PI);
	uint8_t y = psx_stick(PSS_LX);
	result.yaw = (tan((y - 128) / (32 * M_PI))) / (5 * M_PI);
	
	// Throttle is a Relative Linear Control
	// Ignore the points around the centre of the stick
	// Scale into +/- MAX_THROTTLE% change / second range
	// Multiply by -1 to invert; up is more throttle, down is reverse throttle
	// Don't let the throttle exceed 80%; this leaves some overhead to pitch and roll and "full" throttle
	uint8_t t = psx_stick(PSS_LY);
	if (t < 64 || t > 191) {
		double delta = (M_THROTTLE * t + B_THROTTLE) * -1 * (dt * 0.001);
		throttle += delta;
		if (throttle > 1.0) throttle = 1.0;
		if (throttle < 0.0) throttle = 0.0;
	}
	result.throttle = throttle;
	return result;
}

uint16_t control_button_state(){
	uint16_t state = 0;

	if (button_state & PSB_START) state |= POWER;
	if (button_state & PSB_SELECT) state |= TELEMETRY;
	
	if (button_state & PSB_TRIANGLE) state |= MODE_COMMIT;
	if (button_state & PSB_CIRCLE) state |= MODE_NEXT;
	if (button_state & PSB_SQUARE) state |= MODE_PREV;
	if (button_state & PSB_CROSS) state |= MODE_RESET;
	
	if (button_state & PSB_PAD_UP) state |= VALUE_UP;
	if (button_state & PSB_PAD_DOWN) state |= VALUE_DOWN;
	if (button_state & PSB_PAD_LEFT) state |= VALUE_PREV;
	if (button_state & PSB_PAD_RIGHT) state |= VALUE_NEXT;	
	
	return state;
}
uint16_t control_button_state_changed() {
	uint16_t changed = 0;

	if (button_changed & PSB_START) changed |= POWER;
	if (button_changed & PSB_SELECT) changed |= TELEMETRY;

	if (button_changed & PSB_TRIANGLE) changed |= MODE_COMMIT;
	if (button_changed & PSB_CIRCLE) changed |= MODE_NEXT;
	if (button_changed & PSB_SQUARE) changed |= MODE_PREV;
	if (button_changed & PSB_CROSS) changed |= MODE_RESET;
	
	if (button_changed & PSB_PAD_UP) changed |= VALUE_UP;
	if (button_changed & PSB_PAD_DOWN) changed |= VALUE_DOWN;
	if (button_changed & PSB_PAD_LEFT) changed |= VALUE_PREV;
	if (button_changed & PSB_PAD_RIGHT) changed |= VALUE_NEXT;
	
	return changed;
}

void control_reset_throttle() {
	throttle = 0;
}

