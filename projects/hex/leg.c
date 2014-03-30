#include "leg.h"

uint16_t leg_position[12];

int16_t leg_neutral_offset[12] = {
	//Front:
	80,	//L_COXA
	-30,		//L_TIBIA
	0,		//R_COXA
	60,	//R_TIBIA
	
	//Middle:
	0,		//L_COXA
	-50,		//L_TIBIA
	0,		//R_COXA
	80,		//R_TIBIA
	
	//Rear
	0,	//L_COXA
	-50,		//L_TIBIA
	0,	//R_COXA
	100		//R_TIBIA
};

int8_t leg_rotation_direction[12] = {
	//Front:
	1,	//L_COXA
	1,	//L_TIBIA
	-1,	//R_COXA
	-1,	//R_TIBIA
	
	//Middle:
	1,	//L_COXA
	1,	//L_TIBIA
	-1,	//R_COXA
	-1,	//R_TIBIA
	
	//Rear
	1,	//L_COXA
	1,	//L_TIBIA
	-1,	//R_COXA
	-1	//R_TIBIA
};

void update_leg_position(uint16_t delay){
	for (uint8_t i = 0; i < 12; i++){
		pwm_set_phase(i, leg_position[i]);
	}
	_delay_ms(delay);
}

/**
 * Lifts and positions the leg without moving the robot.  Used to initialize legs for a given gait.  Offset is negative for 
 * positions facing forward, and negative for positioned towards the back of the robot.
 */
void lift_and_position_leg(uint8_t leg, int16_t coxa_offset, int16_t tibia_offset){
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (TIBIA_RAISED * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);
	leg_position[leg] = NEUTRAL + leg_neutral_offset[leg] + (coxa_offset * leg_rotation_direction[leg]);
	update_leg_position(DELAY_SHORT);
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (tibia_offset * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);	
}

void leg_init(){
	for (uint8_t i = 0; i < 12; i++){
		leg_position[i] = NEUTRAL + leg_neutral_offset[i];
	}
	
	update_leg_position(2000);
}
