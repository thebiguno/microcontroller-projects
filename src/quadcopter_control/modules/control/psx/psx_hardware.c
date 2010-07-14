/*
 * PSX control hardware implementation
 * Written by Wyatt Olson
 */

#include "../control_hardware.h"
#include "../../../lib/psx/psx.h"
#include "../../../lib/timer/timer.h"

#define CONTROLLER_POLL_MS 10

void init_hardware(){
	psx_init(&PORTD, &PIND, &DDRD, PORTD6, //Data (Brown)
			&PORTD, &DDRD, PORTD5, //Clock (Blue)
			&PORTD, &DDRD, PORTD7, //Command (Orange)
			&PORTB, &DDRB, PORTB0); //Attention (Yellow)

}

/*
 * Polls the controller (at most) once every CONTROLLER_POLL_MS ms.  This method should
 * be called before each getter, but it will only update the data when needed.
 */
uint64_t last_control_update_time = 0;
void poll_controller(){
	if (last_control_update_time + CONTROLLER_POLL_MS < timer_millis()){
		last_control_update_time = timer_millis();
		psx_read_gamepad();
	}
}
 
uint16_t get_pitch(){
	poll_controller();
	
	uint8_t raw_data = psx_stick(PSS_LY);

	//Perform scaling -- TODO different methods	
	int16_t scaled_data = raw_data << 0x8;
	
	return scaled_data;
}

uint16_t get_roll(){
	poll_controller();
	
	uint8_t raw_data = psx_stick(PSS_LX);

	//Perform scaling -- TODO different methods	
	int16_t scaled_data = raw_data << 0x8;
	
	return scaled_data;
}

uint16_t get_yaw(){
	poll_controller();
	
	int16_t scaled_data = 0x7FFF;
	
	uint8_t l1 = psx_button(PSB_L1);
	uint8_t l2 = psx_button(PSB_L2);
	uint8_t r1 = psx_button(PSB_R1);
	uint8_t r2 = psx_button(PSB_R2);
	
	//TODO these numbers are arbitrary, change them to actually make sense
	if (!r1 && !r2){
		if (l1 && l2) scaled_data = 0x0000;
		else if (l2) scaled_data = 0x2000;
		else if (l1) scaled_data = 0x6000;
	}
	else if (!l1 && !l2){
		if (r1 && r2) scaled_data = 0xFFFF;
		else if (r2) scaled_data = 0xDFFF;
		else if (r1) scaled_data = 0x9FFF;	
	}
	
	return scaled_data;
}

uint16_t get_throttle(){
	poll_controller();
	
	uint8_t raw_data = psx_stick(PSS_RY);

	//Perform scaling -- TODO different methods	
	int16_t scaled_data = raw_data << 0x8;
	
	return scaled_data;
}

#define POWER_ON_IMPL 			PSB_START
#define MODE_AEROBATIC_IMPL 	PSB_SQUARE
#define MODE_SPORT_IMPL			PSB_CIRCLE
#define MODE_STABLE_IMPL	 	PSB_CROSS
#define MODE_HOVER_IMPL		 	PSB_TRIANGLE

uint16_t getButtons(){
	uint16_t buttons = 0x0;

	if (psx_button(PSB_START)) buttons |= POWER_ON;
	if (psx_button(PSB_SQUARE)) buttons |= MODE_AEROBATIC;
	if (psx_button(PSB_CIRCLE)) buttons |= MODE_SPORT;
	if (psx_button(PSB_CROSS)) buttons |= MODE_STABLE;
	if (psx_button(PSB_TRIANGLE)) buttons |= MODE_HOVER;
	
	return buttons;
}
