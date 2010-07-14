/*
 * PSX control hardware implementation
 * Written by Wyatt Olson
 */

#include "../control.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/timer/timer.h"

#define CONTROLLER_POLL_MS 10

#define MAX_PITCH M_PI / 3				//Max pitch is ¹/3 (60 degrees)
#define M_PITCH (2 * MAX_PITCH) / 256	//1 unit change in raw pitch is 0.0081... radians change
#define B_PITCH MAX_PITCH * -1			//Zero value

#define MAX_ROLL M_PI / 3				//Max roll is ¹/3 (60 degrees)
#define M_ROLL (2 * MAX_ROLL) / 256		//1 unit change in raw roll is 0.0081... radians change
#define B_ROLL MAX_ROLL * -1			//Zero value

#define R_YAW M_PI / 24					//7.5 degrees / second / input

#define MAX_THROTTLE 1.0				//Max throttle is 1
#define M_THROTTLE 2 * MAX_THROTTLE / 256
#define B_THROTTLE MAX_THROTTLE * -1

void init_control_hardware(){
	psx_init(&PORTD, &PIND, &DDRD, PORTD6, //Data (Brown)
			&PORTD, &DDRD, PORTD5, //Clock (Blue)
			&PORTD, &DDRD, PORTD7, //Command (Orange)
			&PORTB, &DDRB, PORTB0); //Attention (Yellow)

}

/*
 * Polls the controller (at most) once every CONTROLLER_POLL_MS ms.  This method should
 * be called before each getter, but it will only update the data when needed.
 */
uint64_t _last_control_update_time = 0;
void _poll_controller(){
	if (_last_control_update_time + CONTROLLER_POLL_MS < timer_millis()){
		_last_control_update_time = timer_millis();
		psx_read_gamepad();
	}
}
 
double get_pitch(){
	_poll_controller();
	
	uint8_t raw_data = psx_stick(PSS_LY);

	//Perform scaling into radians.  Currently this is linear, we may want 
	// to change it to 
	return M_PITCH * raw_data + B_PITCH;
}

double get_roll(){
	_poll_controller();
	
	uint8_t raw_data = psx_stick(PSS_LX);

	//Perform scaling into radians
	return M_ROLL * raw_data + B_ROLL;
}

double get_yaw(){
	_poll_controller();
	
	double scaled_data = 0.0;
	
	uint8_t l1 = psx_button(PSB_L1);
	uint8_t l2 = psx_button(PSB_L2);
	uint8_t r1 = psx_button(PSB_R1);
	uint8_t r2 = psx_button(PSB_R2);
	
	//TODO these numbers are arbitrary, change them to actually make sense
	if (!r1 && !r2){
		if (l1 && l2) scaled_data = R_YAW * -4;
		else if (l2) scaled_data = R_YAW * -2;
		else if (l1) scaled_data = R_YAW * -1;
	}
	else if (!l1 && !l2){
		if (r1 && r2) scaled_data = R_YAW * 4;
		else if (r2) scaled_data = R_YAW * 2;
		else if (r1) scaled_data = R_YAW * 1;	
	}
	
	return scaled_data;
}

double get_throttle(){
	_poll_controller();
	
	uint8_t raw_data = psx_stick(PSS_RY);

	//Perform scaling into radians
	return (M_THROTTLE * raw_data + B_THROTTLE) * -1; //*-1 to invert -- up is more throttle
}

#define POWER_ON_IMPL 			PSB_START
#define MODE_AEROBATIC_IMPL 	PSB_SQUARE
#define MODE_SPORT_IMPL			PSB_CIRCLE
#define MODE_STABLE_IMPL	 	PSB_CROSS
#define MODE_HOVER_IMPL		 	PSB_TRIANGLE

uint16_t get_buttons(){
	uint16_t buttons = 0x0;

	if (psx_button(PSB_START)) buttons |= POWER_ON;
	if (psx_button(PSB_SQUARE)) buttons |= MODE_AEROBATIC;
	if (psx_button(PSB_CIRCLE)) buttons |= MODE_SPORT;
	if (psx_button(PSB_CROSS)) buttons |= MODE_STABLE;
	if (psx_button(PSB_TRIANGLE)) buttons |= MODE_HOVER;
	
	return buttons;
}
