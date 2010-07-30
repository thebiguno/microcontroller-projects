/*
 * PSX control hardware implementation
 * Written by Wyatt Olson
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

//Mappings between button functions and PSX buttons
#define POWER_ON_IMPL 			PSB_START
#define MODE_AEROBATIC_IMPL 	PSB_SQUARE
#define MODE_SPORT_IMPL			PSB_CIRCLE
#define MODE_STABLE_IMPL	 	PSB_CROSS
#define MODE_HOVER_IMPL		 	PSB_TRIANGLE


void control_init(){
	psx_init(&PORTD, &PIND, &DDRD, PORTD6, //Data (Brown)
			&PORTD, &DDRD, PORTD5, //Clock (Blue)
			&PORTD, &DDRD, PORTD7, //Command (Orange)
			&PORTB, &DDRB, PORTB0); //Attention (Yellow)

}
 
control_t get_control(){
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
		if (l1 && l2) result.yaw = R_YAW * -4;
		else if (l2) result.yaw = R_YAW * -2;
		else if (l1) result.yaw = R_YAW * -1;
	}
	else if (!l1 && !l2){
		if (r1 && r2) result.yaw = R_YAW * 4;
		else if (r2) result.yaw = R_YAW * 2;
		else if (r1) result.yaw = R_YAW * 1;	
	}
	
	//Scale into -1 .. 1 range.  We multiply the entire thing by -1 to invert;
	// up is more throttle, down is reverse throttle
	result.throttle = (M_THROTTLE * psx_stick(PSS_RY) + B_THROTTLE) * -1;
	
	return result;
}

uint16_t get_buttons(){
	uint16_t buttons = 0x0;

	if (psx_button(PSB_START)) buttons |= POWER_ON;
	if (psx_button(PSB_SQUARE)) buttons |= MODE_AEROBATIC;
	if (psx_button(PSB_CIRCLE)) buttons |= MODE_SPORT;
	if (psx_button(PSB_CROSS)) buttons |= MODE_STABLE;
	if (psx_button(PSB_TRIANGLE)) buttons |= MODE_HOVER;
	
	return buttons;
}
