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

void control_init(){
	psx_init(&PORT_PSX_DATA, PIN_PSX_DATA, //Data (Brown)
			&PORT_PSX_CLOCK, PIN_PSX_CLOCK, //Clock (Blue)
			&PORT_PSX_COMMAND, PIN_PSX_COMMAND, //Command (Orange)
			&PORT_PSX_ATTENTION, PIN_PSX_ATTENTION); //Attention (Yellow)
}
 
control_t control_read_analog(){
	psx_read_gamepad();
	
	control_t result;

	//Perform scaling into radians.  Currently this is linear, we may want 
	// to change it to 
	result.pitch = M_PITCH * psx_stick(PSS_LY) + B_PITCH;

	//Perform scaling into radians
	result.roll = M_ROLL * psx_stick(PSS_LX) + B_ROLL;
	
	result.yaw = 0.0;
	
	uint8_t l1 = psx_button(PSB_L1);
	uint8_t l2 = psx_button(PSB_L2);
	uint8_t r1 = psx_button(PSB_R1);
	uint8_t r2 = psx_button(PSB_R2);

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
	
	//Scale into -1 .. 1 range.  We multiply the entire thing by -1 to invert;
	// up is more throttle, down is reverse throttle
	result.throttle = (M_THROTTLE * psx_stick(PSS_RY) + B_THROTTLE) * -1;
	return result;
}

uint16_t control_read_buttons(){
	uint16_t buttons = 0x0;

	if (psx_button(PSB_START)) buttons |= POWER;
	if (psx_button(PSB_SELECT)) buttons |= CRUISE;
	if (psx_button(PSB_L3)) buttons |= RESET_ATTITUDE;
	if (psx_button(PSB_R3)) buttons |= CALIBRATE;
	if (psx_button(PSB_SQUARE)) buttons |= MODE_AEROBATIC;
	if (psx_button(PSB_CIRCLE)) buttons |= MODE_SPORT;
	if (psx_button(PSB_CROSS)) buttons |= MODE_STABLE;
	if (psx_button(PSB_TRIANGLE)) buttons |= MODE_HOVER;
	
	return buttons;
}

